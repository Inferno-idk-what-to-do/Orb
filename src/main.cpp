#include "GlobalNamespace/VRController.hpp"
#include "GlobalNamespace/FlickeringNeonSign.hpp"
#include "GlobalNamespace/MainMenuViewController.hpp"

#include "UnityEngine/GameObject.hpp"
#include "UnityEngine/PrimitiveType.hpp"
#include "UnityEngine/Transform.hpp"
#include "UnityEngine/AssetBundle.hpp"
#include "UnityEngine/AssetBundleCreateRequest.hpp"
#include "UnityEngine/AssetBundleRequest.hpp"

#include "questui/shared/QuestUI.hpp"

#include "main.hpp"
#include "config.hpp"
#include "SettingsViewController.hpp"
#include "assets.hpp"

using namespace GlobalNamespace;
using namespace UnityEngine;

DEFINE_CONFIG(ModConfig);

static ModInfo modInfo; // Stores the ID and version of our mod, and is sent to the modloader upon startup

// Loads the config from disk using our modInfo, then returns it for use
Configuration& getConfig() {
    static Configuration config(modInfo);
    config.Load();
    return config;
}

// Returns a logger, useful for printing debug messages
Logger& getLogger() {
    static Logger* logger = new Logger(modInfo);
    return *logger;
}

bool isLeftSaber = true;

Vector3 leftPos, rightPos, avg, offset;

GameObject *sphere = nullptr;
auto size = Vector3(.2f, .2f, .2f);

MAKE_HOOK_MATCH(VRCUpdateHook, &VRController::Update, void, VRController *self)
{
    VRCUpdateHook(self);

    if(sphere)
    {
        if(isLeftSaber) 
        {
            leftPos = self->get_position();
        }
        else 
        {
            rightPos = self->get_position();
        }

        avg = Vector3(((leftPos.x + rightPos.x) / 2 + offset.x), ((leftPos.y + rightPos.y) / 2 + offset.y), ((leftPos.z + rightPos.z) / 2 + offset.z));
        sphere->get_transform()->set_position(avg);

        // ! update config stuff
        // enabled
        if(sphere->get_active() != getModConfig().enabled.GetValue())
        {
            sphere->set_active(getModConfig().enabled.GetValue());
        }
        // size
        if(sphere->get_transform()->get_localScale().x != getModConfig().size.GetValue() * .2f)
        {
            size.x = .2f * getModConfig().size.GetValue();
            size.y = .2f * getModConfig().size.GetValue();
            size.z = .2f * getModConfig().size.GetValue();

            sphere->get_transform()->set_localScale(size);
        }
        // offset
        offset = getModConfig().offset.GetValue();
    }   

    isLeftSaber = !isLeftSaber;
}

custom_types::Helpers::Coroutine LoadOrbBundle()
{
    getLogger().info("line 88");
    using AssetBundle_LoadFromMemoryAsync = function_ptr_t<UnityEngine::AssetBundleCreateRequest*, ArrayW<uint8_t>, int>;
    static AssetBundle_LoadFromMemoryAsync assetBundle_LoadFromMemoryAsync = reinterpret_cast<AssetBundle_LoadFromMemoryAsync>(il2cpp_functions::resolve_icall("UnityEngine.AssetBundle::LoadFromMemoryAsync_Internal"));

    getLogger().info("line 92");
    auto bundleReq = assetBundle_LoadFromMemoryAsync(IncludedAssets::orbofpondering, 0);
    getLogger().info("line 94");
    bundleReq->set_allowSceneActivation(true);

    getLogger().info("line 97");
    auto bundle = bundleReq->get_assetBundle();
    getLogger().info("line 99");
    Object::DontDestroyOnLoad(bundle);

    getLogger().info("line 102");
    auto assetReq = bundle->LoadAssetAsync("orbofpondering", reinterpret_cast<System::Type*>(csTypeOf(GameObject*)));
    getLogger().info("line 104");
    assetReq->set_allowSceneActivation(true);

    getLogger().info("line 107");
    auto asset = reinterpret_cast<GameObject*>(assetReq->get_asset());
    getLogger().info("line 109");
    auto orb = Object::Instantiate(asset);
    getLogger().info("line 111");
    Object::DontDestroyOnLoad(orb);
    getLogger().info("line 113");
    orb->SetActive(true);
    getLogger().info("line 115");

    co_return;
}

MAKE_HOOK_MATCH(SignHook, &FlickeringNeonSign::Start, void, FlickeringNeonSign *self)
{
    SignHook(self);

    sphere = GameObject::CreatePrimitive(PrimitiveType::Sphere);

    size.x *= getModConfig().size.GetValue();
    size.y *= getModConfig().size.GetValue();
    size.z *= getModConfig().size.GetValue();
    sphere->get_transform()->set_localScale(size);

    getLogger().info("Starting load bundle");
    self->StartCoroutine(custom_types::Helpers::CoroutineHelper::New(LoadOrbBundle()));
    getLogger().info("Bundle loaded");

    offset = getModConfig().offset.GetValue();
}

// Called at the early stages of game loading
extern "C" void setup(ModInfo& info) {
    info.id = ID;
    info.version = VERSION;
    modInfo = info;
	
    getConfig().Load(); // Load the config file
    getLogger().info("Completed setup!");
}

// Called later on in the game loading - a good time to install function hooks
extern "C" void load() {
    il2cpp_functions::Init();

    getModConfig().Init(modInfo);

    getLogger().info("Installing hooks...");
    
    INSTALL_HOOK(getLogger(), VRCUpdateHook);
    INSTALL_HOOK(getLogger(), SignHook);

    QuestUI::Register::RegisterModSettingsViewController(modInfo, DidActivate);

    getLogger().info("Installed all hooks!");
}