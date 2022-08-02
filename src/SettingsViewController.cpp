// Shuotout darknight for doing all of the hard shit

#include "main.hpp"
#include "config.hpp"
#include "SettingsViewController.hpp"

#include "HMUI/Touchable.hpp"

#include "questui/shared/BeatSaberUI.hpp"

#include "config-utils/shared/config-utils.hpp"

using namespace QuestUI;
using namespace UnityEngine;
using namespace UnityEngine::UI;
using namespace HMUI;
using namespace ConfigUtils;

void DidActivate(ViewController* self, bool firstActivation, bool addedToHierarchy, bool screenSystemEnabling) {
    if(firstActivation) {
        self->get_gameObject()->AddComponent<Touchable*>();

        GameObject* container = BeatSaberUI::CreateScrollableSettingsContainer(self->get_transform());
        Transform* parent = container->get_transform();

        auto layout = BeatSaberUI::CreateVerticalLayoutGroup(parent);
        layout->GetComponent<LayoutElement*>()->set_preferredWidth(90.0f);
        layout->set_childControlWidth(true);
        auto layoutParent = layout->get_transform();

        AddConfigValueToggle(parent, getModConfig().enabled);
        AddConfigValueIncrementVector3(parent, getModConfig().offset, 1, 0.1f);
        AddConfigValueIncrementDouble(parent, getModConfig().size, 1, 0.1, 0.1, 5.0);
    }
}