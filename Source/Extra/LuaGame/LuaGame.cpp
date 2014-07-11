
#include <Pxf/Extra/LuaGame/LuaGame.h>
#include <Pxf/Extra/LuaGame/Subsystems/Vec2.h>
#include <Pxf/Extra/LuaGame/Subsystems/Graphics.h>
#include <Pxf/Extra/LuaGame/Subsystems/Resources.h>
#include <Pxf/Extra/LuaGame/Subsystems/Physics.h>
#if defined(TARGET_OS_IPHONEDEV)
	#include <Pxf/Extra/LuaGame/Subsystems/iPhoneInput.h>
#else
	#include <Pxf/Extra/LuaGame/Subsystems/MouseInput.h>
#endif

#include <Pxf/Graphics/Texture.h>
extern "C"
{
    #include <luasocket.h>
}


#define LOCAL_MSG "LuaGame"

using namespace Pxf;
using namespace Pxf::Extra;

LuaGame::Game::Game(Util::String _gameFilename, Graphics::Device* _device, bool _gracefulFail)
{
    m_Device = _device;

    m_GameFilename = _gameFilename;
    m_GameIdent = "Unknown Game";

    // Reset version
    m_GameVersion = "0.0.0";

    m_Running = false;
    m_HasInitialized = false;
    m_GracefulFail = _gracefulFail;

    // Graphics
    //m_QuadBatch = m_Device->CreateQuadBatch(1024);
    //m_QBT = new QBTConnection[8];
    m_DepthStep = 0.1f;
    m_DepthFar = -1.0f;
    m_DepthNear = 0.0f;
    m_CurrentDepth = m_DepthFar;

    // QuadBatch-Texture-connections
    m_CurrentQBT = -1;
    m_QBTCount = 0;

    // Preload stuff
    m_PreLoadQueue_Textures_Counter = 0;
    m_PreLoadQueue_Total = -1;

    // Error/panic handling
    m_CrashRetries = 0;
}

LuaGame::Game::~Game()
{
    // Destructor of Game
    CleanUp();
}

void LuaGame::Game::CleanUp()
{
    delete m_CoreTexture;
    delete m_CoreQB;

    for(int i = 0; i < m_QBTCount; ++i)
    {
        delete m_QBT[i]->m_Texture;
        delete m_QBT[i]->m_QuadBatch;
    }

    m_CurrentQBT = -1;
    m_QBTCount = 0;
    m_PreLoadQueue_Textures_Counter = 0;
    m_PreLoadQueue_Total = -1;
}

bool LuaGame::Game::Reload()
{
    CleanUp();
    return Load();
}

bool LuaGame::Game::Load()
{

    m_HasInitialized = false;

    // Load core textures
    m_CoreTexture = m_Device->CreateTexture("error_msg.png");
    m_CoreQB      = m_Device->CreateQuadBatch(256);

    // Init lua state
    L = lua_open();

    // Register lua libs
    _register_lua_libs_callbacks();

    // Register own callbacks
    _register_own_callbacks();

    ////////////////////////////////
    // Load game core/helper script
    int s = luaL_loadfile(L, "LuaGame.lua");

    if (!s)
	{
		s = lua_pcall(L, 0, LUA_MULTRET, 0);
		if ( s ) {
			Message(LOCAL_MSG, " [Error while running LuaGame.lua] -- %s", lua_tostring(L, -1));
			lua_pop(L, 1); // remove error message
			m_Running = false;
		} else {

		    // Set Game.Instance to this class instance!
		    // Instance will later be used to call correct Game instance.
            lua_getglobal(L, LUAGAME_TABLE);
            lua_pushlightuserdata(L, this);
            lua_setfield(L, -2, "Instance");
            lua_pop(L, 1);

		    // Call core init function (Game:CoreInit())
		    if (!CallGameMethod("CoreInit"))
                return false;

		    //////////////////////////
			// Load main game script
            s = luaL_loadfile(L, m_GameFilename.c_str());
        	if (!s)
        	{
        		s = lua_pcall(L, 0, LUA_MULTRET, 0);

        		if ( s ) {
        			Message(LOCAL_MSG, "-- %s", lua_tostring(L, -1));
        			lua_pop(L, 1);
        		} else {
        		    // Call Game:PreLoad()
                    if (CallGameMethod("PreLoad"))
                    {
                        m_Running = true;
                    } else {
                        Message(LOCAL_MSG, "Failed in PreLoad phase.");
                    }
        		}

        	} else {
        		Message(LOCAL_MSG, "Error: %s", lua_tostring(L,-1));
                m_Running = false;
        	}
		}

	} else {
		Message(LOCAL_MSG, "Error while loading Game.lua: %s",lua_tostring(L,-1));
		m_Running = false;
	}

    return true;
}

int LuaGame::Game::PreLoad()
{
    // Preload game data
    if (!m_Running)
        return false;

    // Return value
    int ret = m_PreLoadQueue_Textures_Counter;// + m_PreLoadQueue_Sound_Counter;
    if (ret == 0)
        return 0;

    // Load textures
    if (m_PreLoadQueue_Textures_Counter > 0)
    {

        m_PreLoadQueue_Textures_Counter -= 1;
        m_PreLoadQueue_Textures[m_PreLoadQueue_Textures_Counter]->Load();

        m_CurrentQBT += 1;
        m_QBTCount = m_CurrentQBT + 1;
        m_QBT[m_CurrentQBT] = new QBTConnection(m_PreLoadQueue_Textures[m_PreLoadQueue_Textures_Counter], m_Device);

    ////////////////////
    // TODO: Load sounds
    /*
    } else if () {

    */
    }

    // Returns how many preload data pieces there are left
    return ret;
}

Graphics::Texture* LuaGame::Game::AddPreload(Util::String _filepath)
{
    // Now we got one more preload entry in the queue!
    Graphics::Texture* res = m_Device->CreateTexture(_filepath.c_str(), false); // false = dont autoload
    m_PreLoadQueue_Textures[m_PreLoadQueue_Textures_Counter] = res;
    m_PreLoadQueue_Textures_Counter += 1;

    return res;
}

bool LuaGame::Game::Update(float dt)
{
    // Update game
    if (!m_Running || !m_HasInitialized)
        return false;

    // Update iPhone input handling
#if defined(TARGET_OS_IPHONEDEV)
    IPhoneInputSubsystem::Update(this, L);
#else
    MouseInputSubsystem::Update(this, L);
#endif



    lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);
	lua_getglobal(L, LUAGAME_TABLE);
    lua_getfield(L, -1, "Update");
    lua_remove(L, -2);
    lua_getglobal(L, LUAGAME_TABLE);
    lua_pushnumber(L, dt);
	m_Running = HandleLuaErrors(lua_pcall(L, 2, 0, -4));



    return m_Running;
}

bool LuaGame::Game::Render()
{

    // Render game
    if (!m_Running)
    {
        if (m_CrashRetries < 3)
        {
            m_CrashRetries += 1;
            m_Running = true;
            // Try to restart the script
            Message(LOCAL_MSG, "Script has stopped running. Trying to restart... (Retry # %i)", m_CrashRetries);
            CleanUp();
            Load();

        } else {

            // Script has encountered an error
            // Display panic msg!
            m_CoreQB->Reset();

            int w,h;
            #if defined(TARGET_OS_IPHONEFAKEDEV)
                    w = m_HitArea[2];
                    h = m_HitArea[3];
            #else
                    m_Device->GetSize(&w, &h);
            #endif

            Math::Vec3f t_color_white(1.0f, 1.0f, 1.0f);
            Math::Vec3f t_color_black(0.0f, 0.0f, 0.0f);

            // bg
            m_CurrentDepth = m_DepthFar;
            m_Device->BindTexture(0);
            m_CoreQB->SetDepth(m_CurrentDepth);
            m_CoreQB->SetColor(&t_color_black);
            m_CoreQB->AddTopLeft(0, 0, w, h);
            m_CurrentDepth += m_DepthStep;

            // Msg
            m_Device->BindTexture(m_CoreTexture);
            m_CoreQB->SetColor(&t_color_white);
            m_CoreQB->SetTextureSubset(0.0f, 0.0f, 1.0f, 1.0f);
            m_CoreQB->SetDepth(m_CurrentDepth);
            m_CoreQB->AddCentered(w / 2.0f, h / 2.0f, 256, 256);
            m_CoreQB->Draw();

        }
        return false;
    }

    // Are we in need of preloading anything?
    int t_preload = PreLoad();
    if (m_PreLoadQueue_Total == -1)
    {
        m_PreLoadQueue_Total = t_preload;

        // Calculate depth step
        if (m_PreLoadQueue_Total > 0)
            m_DepthStep = (1.0f / m_PreLoadQueue_Total) / 1024;
    }

    if (t_preload > 0)
    {

        int w,h;
#if defined(TARGET_OS_IPHONEFAKEDEV)
        w = m_HitArea[2];
        h = m_HitArea[3];
#else
        m_Device->GetSize(&w, &h);
#endif

        /*

        +------------------------+
        |############            |
        +------------------------+

        */

        Math::Vec3f t_color_white(1.0f, 1.0f, 1.0f);
        Math::Vec3f t_color_black(0.0f, 0.0f, 0.0f);
        Math::Vec3f t_color_red(1.0f, 0.0f, 0.0f);


        m_Device->BindTexture(0);
        m_CoreQB->Reset();

        m_CoreQB->SetTextureSubset(0.0f, 0.0f, 1.0f, 1.0f);
        m_CoreQB->SetDepth(m_CurrentDepth);

        // bg
        m_CoreQB->SetColor(&t_color_black);
        m_CoreQB->AddTopLeft(0, 0, w, h);

        // bar outline
        float t_bar_width = w * 0.8f;
        float t_bar_height = 8.0f;
        m_CoreQB->SetColor(&t_color_white);
        m_CoreQB->AddCentered(w / 2.0f, h / 2.0f, t_bar_width, t_bar_height);

        // bar bg
        m_CoreQB->SetColor(&t_color_black);
        m_CoreQB->AddCentered(w / 2.0f, h / 2.0f, t_bar_width-2.0f, t_bar_height-2.0f);

        // bar ticks
        m_CoreQB->SetColor(&t_color_white);
        m_CoreQB->AddTopLeft((w - t_bar_width + 4.0f) / 2.0f, (h - t_bar_height + 4.0f) / 2.0f, (t_bar_width - 4.0f) * ((float)(m_PreLoadQueue_Total - t_preload + 1) / (float)m_PreLoadQueue_Total), t_bar_height - 4.0f);

        m_CoreQB->Draw();

        Message(LOCAL_MSG, "Loaded resource %i/%i.", (m_PreLoadQueue_Total - t_preload + 1), m_PreLoadQueue_Total);

        // Call Game:Init()
        if ((m_PreLoadQueue_Total - t_preload + 1) == m_PreLoadQueue_Total)
        {
            m_Running = CallGameMethod("Init");
            m_HasInitialized = true;
        }
    } else {
        // Reset depth
        m_CurrentDepth = m_DepthFar;

        // Loop all quad batches
        for(int i = 0; i < m_QBTCount; ++i)
            m_QBT[i]->m_QuadBatch->Reset();

        m_Running = CallGameMethod("Render");

        for(int i = 0; i < m_QBTCount; ++i)
        {
            m_Device->BindTexture(m_QBT[i]->m_Texture);
            m_QBT[i]->m_QuadBatch->Draw();
        }

    }

    return m_Running;
}

Graphics::QuadBatch* LuaGame::Game::GetCurrentQB()
{
    return m_QBT[m_CurrentQBT]->m_QuadBatch;
}

Graphics::Texture* LuaGame::Game::GetCurrentTexture()
{
    return m_QBT[m_CurrentQBT]->m_Texture;
}

void LuaGame::Game::BindTexture(Graphics::Texture* _texture)
{
    for(size_t i = 0; i < m_QBTCount; ++i)
    {
        if (m_QBT[i]->m_Texture == _texture)
        {
            m_CurrentQBT = i;
        }
    }
}

void LuaGame::Game::AddQuad(float x, float y, float w, float h)
{
	GetCurrentQB()->SetTextureSubset(0.0f, 0.0f, 1.0f, 1.0f);
    GetCurrentQB()->SetDepth(m_CurrentDepth);
    GetCurrentQB()->AddCentered(x, y, w, h);
    m_CurrentDepth += m_DepthStep;
}

void LuaGame::Game::AddQuad(float x, float y, float w, float h, float s0, float t0, float s1, float t1)
{
    // Note: Texture coords are in pixels
    Math::Vec4f coords = GetCurrentTexture()->CreateTextureSubset(s0, t0, s1, t1);
	GetCurrentQB()->SetTextureSubset(coords.x, coords.y, coords.z, coords.w);
    GetCurrentQB()->SetDepth(m_CurrentDepth);
    GetCurrentQB()->AddCentered(x, y, w, h);
    m_CurrentDepth += m_DepthStep;
}

void LuaGame::Game::AddQuad(float x, float y, float w, float h, float rotation)
{
    GetCurrentQB()->SetTextureSubset(0.0f, 0.0f, 1.0f, 1.0f);
    GetCurrentQB()->SetDepth(m_CurrentDepth);
    GetCurrentQB()->AddCentered(x, y, w, h, rotation);
    m_CurrentDepth += m_DepthStep;
}

void LuaGame::Game::AddQuad(float x, float y, float w, float h, float rotation, float s0, float t0, float s1, float t1)
{
    // Note: Texture coords are in pixels
    Math::Vec4f coords = GetCurrentTexture()->CreateTextureSubset(s0, t0, s1, t1);
	GetCurrentQB()->SetTextureSubset(coords.x, coords.y, coords.z, coords.w);
    GetCurrentQB()->SetDepth(m_CurrentDepth);
    GetCurrentQB()->AddCentered(x, y, w, h, rotation);
    m_CurrentDepth += m_DepthStep;
}

void LuaGame::Game::SetColor(float r, float g, float b)
{
    for(int i = 0; i < m_QBTCount; ++i)
        m_QBT[i]->m_QuadBatch->SetColor(r, g, b);
}

void LuaGame::Game::SetAlpha(float a)
{
    for(int i = 0; i < m_QBTCount; ++i)
        m_QBT[i]->m_QuadBatch->SetAlpha(a);
}

void LuaGame::Game::Translate(float x, float y)
{
    for(int i = 0; i < m_QBTCount; ++i)
        m_QBT[i]->m_QuadBatch->Translate(x, y);
}

void LuaGame::Game::Rotate(float angle)
{
    for(int i = 0; i < m_QBTCount; ++i)
        m_QBT[i]->m_QuadBatch->Rotate(angle);
}

void LuaGame::Game::LoadIdentity()
{
    for(int i = 0; i < m_QBTCount; ++i)
        m_QBT[i]->m_QuadBatch->LoadIdentity();
}

///////////////////////////////////////////////////////////////////
// Private methods
void LuaGame::Game::_register_lua_libs_callbacks()
{
	// Lua libs
	static const luaL_Reg lualibs[] = {
		{"", luaopen_base},
		{LUA_LOADLIBNAME, luaopen_package},
		{LUA_TABLIBNAME, luaopen_table},
		//{LUA_IOLIBNAME, luaopen_io},
		//{LUA_OSLIBNAME, luaopen_os},
		{LUA_STRLIBNAME, luaopen_string},
		{LUA_MATHLIBNAME, luaopen_math},
		{LUA_DBLIBNAME, luaopen_debug},
		{NULL, NULL}
	};

	for (const luaL_Reg *lib = lualibs; lib->func; lib++) {
		lua_pushcfunction(L, lib->func);
		lua_pushstring(L, lib->name);
		lua_call(L, 1, 0);
	}
}

void LuaGame::Game::_register_own_callbacks()
{
    // Register own callbacks
	lua_register(L, "print", Print);
	lua_register(L, "runstring", RunString);
	//lua_register(L, "print", TestInstance);

	/*
	lua_getglobal(L, LUAGAME_TABLE);
    lua_getglobal(L, LUAGAME_TABLE);
    lua_pushcfunction(L, TestInstance);
    lua_setfield(L, -2, "Test");

    // Pop "LuaGame" table
    lua_pop(L, 1);
    */

    // Create empty luagame table
    lua_newtable(L);
    lua_setglobal (L, LUAGAME_TABLE);

    Message(LOCAL_MSG, "Hey");

    // Register subsystems
	Vec2::RegisterClass(L);
	Message(LOCAL_MSG, "Hey");
    GraphicsSubsystem::RegisterClass(L);
    Message(LOCAL_MSG, "Hey");
    ResourcesSubsystem::RegisterClass(L);
    Message(LOCAL_MSG, "Hey");
	PhysicsSubsystem::RegisterClass(L);
	Message(LOCAL_MSG, "Hey");
#if defined(TARGET_OS_IPHONEDEV)
    IPhoneInputSubsystem::RegisterClass(L);
#else
    MouseInputSubsystem::RegisterClass(L);
#endif

    // Sockets
    luaopen_socket_core(L);
}

bool LuaGame::Game::HandleLuaErrors(int _error)
{
	if (_error != 0)
	{
		if (_error == LUA_ERRRUN)
			Message(LOCAL_MSG, "-- Script runtime error: --");
		else if (_error == LUA_ERRMEM)
			Message(LOCAL_MSG, "-- Script memory allocation error: --");
		else
			Message(LOCAL_MSG, "-- Fatal script error: --");
		Message(LOCAL_MSG, "%s", lua_tostring(L, -1));

		lua_pop(L, 1); // remove error message
		m_Running = false;
		return false;
	}
	return true;
}

void LuaGame::Game::RunScriptMethod(int _param_num)
{
    m_Running = CallGameMethod(_param_num);
}

void LuaGame::Game::PrefixStack(const char *_method)
{
    lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);
	lua_getglobal(L, LUAGAME_TABLE);
    lua_getfield(L, -1, _method);
    lua_remove(L, -2);
    lua_getglobal(L, LUAGAME_TABLE);
}

bool LuaGame::Game::CallGameMethod(int _param_num)
{
    /*lua_getglobal(L, "debug");
	lua_getfield(L, -1, "traceback");
	lua_remove(L, -2);
	lua_getglobal(L, LUAGAME_TABLE);
    lua_getfield(L, -1, _method);
    lua_remove(L, -2);
    lua_getglobal(L, LUAGAME_TABLE);

    if (_param != NULL)
    {
        lua_pushstring(L, _param);
        return HandleLuaErrors(lua_pcall(L, _param_num, 0, -3));
    }
    */

	return HandleLuaErrors(lua_pcall(L, 1+_param_num, 0, -3-(_param_num)));
}

bool LuaGame::Game::CallGameMethod(const char* _method)
{
    PrefixStack(_method);
    return CallGameMethod(0);
}

void* LuaGame::Game::GetInstance(lua_State *_L)
{
    lua_getglobal(_L, LUAGAME_TABLE);
    lua_getfield(_L, -1, "Instance");
    static void* p = lua_touserdata(_L, -1);
    lua_pop(_L, 2);

    return p;
}

#if !defined(TARGET_OS_IPHONEDEV)
void LuaGame::Game::SetInputDevice(Input::Input* _InputDevice)
{
    m_InputDevice = _InputDevice;
    //MouseInputSubsystem::RegisterInputInstance(m_InputDevice, L);
}
#endif

#if defined(TARGET_OS_IPHONEFAKEDEV)
void LuaGame::Game::SetHitArea(float x, float y, float w, float h)
{
    m_HitArea[0] = x;
    m_HitArea[1] = y;
    m_HitArea[2] = w;
    m_HitArea[3] = h;
}
#endif

///////////////////////////////////////////////////////////////////
// Callback methods

int LuaGame::Game::Print(lua_State *_L)
{

    // More or less copy paste from lbaselib.c of Lua dist.
    // Modified so that prints can be pushed to the "game console"
    int n = lua_gettop(_L);  /* number of arguments */
    int i;
    lua_getglobal(_L, "tostring");
    for (i=1; i<=n; i++) {
        const char *s;
        lua_pushvalue(_L, -1);  /* function to be called */
        lua_pushvalue(_L, i);   /* value to print */
        lua_call(_L, 1, 1);
        s = lua_tostring(_L, -1);  /* get result */
        if (s == NULL)
            return luaL_error(_L, LUA_QL("tostring") " must return a string to "
                                  LUA_QL("print"));
        if (i>1)
            fputs("\t", stdout);
        fputs("> ", stdout);
        fputs(s, stdout);
        lua_pop(_L, 1);  /* pop result */
    }
    fputs("\n", stdout);
    return 0;
}

int LuaGame::Game::RunString(lua_State *_L)
{
    // runstring(str)
    int argc = lua_gettop(_L);
    if (argc == 1 && lua_isstring(_L, 1))
    {
        if (luaL_dostring(_L, lua_tostring(_L, 1)))
        {
			//Message(LOCAL_MSG, " [Error] -- %s", lua_tostring(_L, -1));
            return 1;
		}
        return (lua_gettop(_L) - argc);

    } else {
        // Non valid method call
        lua_pushstring(_L, "Invalid argument passed to run function!");
        lua_error(_L);
    }

    return 0;
}

