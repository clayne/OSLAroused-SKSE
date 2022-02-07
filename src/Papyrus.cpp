#include "Papyrus.h"
#include "ArousalManager.h"
#include "Serialization.h"
#include "RuntimeEvents.h"
#include "Settings.h"
#include "Debug.h"
#include "SceneManager.h"

void Papyrus::UpdatePlayerNudityCheck(RE::StaticFunctionTag*, bool newVal)
{
	Settings::GetSingleton()->SetPlayerNudityCheckEnabled(newVal);
}

void Papyrus::UpdateHourlyNudityArousalModifier(RE::StaticFunctionTag*, float newVal)
{
	Settings::GetSingleton()->SetHourlyNudityArousalModifier(newVal);
}

void Papyrus::UpdateDefaultArousalMultiplier(RE::StaticFunctionTag*, float newMultiplier)
{
	Settings::GetSingleton()->SetDefaultArousalMultiplier(newMultiplier);
}

float Papyrus::GetArousal(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return ArousalManager::GetArousal(actorRef);
}

std::vector<float> Papyrus::GetArousalMultiple(RE::StaticFunctionTag*, RE::reference_array<RE::Actor*> actorRefs)
{
	std::vector<float> results;

	for (const auto actorRef : actorRefs) {
		results.push_back(ArousalManager::GetArousal(actorRef));
	}

	return results;
}

float Papyrus::GetArousalNoSideEffects(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return ArousalManager::GetArousal(actorRef, false);
}

float Papyrus::SetArousal(RE::StaticFunctionTag*, RE::Actor* actorRef, float value)
{
	return ArousalManager::SetArousal(actorRef, value);
}

void Papyrus::SetArousalMultiple(RE::StaticFunctionTag*, RE::reference_array<RE::Actor*> actorRefs, float value)
{
	for (const auto actorRef : actorRefs) {
		ArousalManager::SetArousal(actorRef, value);
	}
}

float Papyrus::ModifyArousal(RE::StaticFunctionTag*, RE::Actor* actorRef, float value)
{
	return ArousalManager::ModifyArousal(actorRef, value);
}

void Papyrus::ModifyArousalMultiple(RE::StaticFunctionTag*, RE::reference_array<RE::Actor*> actorRefs, float value)
{
	for (const auto actorRef : actorRefs) {
		ArousalManager::ModifyArousal(actorRef, value);
	}
}

void Papyrus::SetArousalMultiplier(RE::StaticFunctionTag*, RE::Actor* actorRef, float value)
{
	//Just clamp between 0 and 100?
	value = std::clamp(value, 0.0f, 100.f);

	Serialization::MultiplierData::GetSingleton()->SetData(actorRef->formID, value);
}

float Papyrus::GetArousalMultiplier(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return Serialization::MultiplierData::GetSingleton()->GetData(actorRef->formID, Settings::GetSingleton()->GetDefaultArousalMultiplier());
}

float Papyrus::ModifyArousalMultiplier(RE::StaticFunctionTag*, RE::Actor* actorRef, float value)
{
	float curMult = Serialization::MultiplierData::GetSingleton()->GetData(actorRef->formID, Settings::GetSingleton()->GetDefaultArousalMultiplier());
	float newVal = curMult + value;
	Serialization::MultiplierData::GetSingleton()->SetData(actorRef->formID, newVal);
	return newVal;
}

float Papyrus::GetExposure(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	//If we are in sla mode get exposure, otherwise just return arousal
	float curTime = RE::Calendar::GetSingleton()->GetCurrentGameTime();
	auto lastCheckTime = Serialization::LastCheckTimeData::GetSingleton()->GetData(actorRef->formID, 0.f);
	Serialization::LastCheckTimeData::GetSingleton()->SetData(actorRef->formID, curTime);
	return ArousalManager::GetSexlabExposure(actorRef, curTime - lastCheckTime);
}

float Papyrus::GetDaysSinceLastOrgasm(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	float lastOrgasmTime = Serialization::LastOrgasmTimeData::GetSingleton()->GetData(actorRef->formID, 0.f);
	if (lastOrgasmTime < 0) {
		lastOrgasmTime = 0;
	}

	return RE::Calendar::GetSingleton()->GetCurrentGameTime() - lastOrgasmTime;
}

void Papyrus::SetTimeRate(RE::StaticFunctionTag*, RE::Actor* actorRef, float value)
{
	if (!actorRef) {
		return;
	}
	value = std::clamp(value, 0.f, 100.f);
	Serialization::TimeRateData::GetSingleton()->SetData(actorRef->formID, value);
}

float Papyrus::GetTimeRate(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return Serialization::TimeRateData::GetSingleton()->GetData(actorRef->formID, 10.0);
}

bool Papyrus::IsActorNaked(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	return Utilities::Actor::IsNaked(actorRef);
}

void Papyrus::RegisterSceneStart(RE::StaticFunctionTag*, bool bIsOstim, int sceneId, RE::reference_array<RE::Actor*> actorRefs)
{
	SceneManager::SceneData sceneData {
		bIsOstim ? SceneManager::SceneFramework::kOStim : SceneManager::SceneFramework::kSexLab,
		sceneId,
		actorRefs
	};
	SceneManager::GetSingleton()->RegisterScene(sceneData);
}

void Papyrus::RemoveScene(RE::StaticFunctionTag*, bool bIsOstim, int sceneId)
{
	SceneManager::GetSingleton()->RemoveScene(bIsOstim ? SceneManager::SceneFramework::kOStim : SceneManager::SceneFramework::kSexLab, sceneId);
}

void Papyrus::RegisterActorOrgasm(RE::StaticFunctionTag*, RE::Actor* actorRef)
{
	Serialization::LastOrgasmTimeData::GetSingleton()->SetData(actorRef->formID, RE::Calendar::GetSingleton()->GetCurrentGameTime());
}

bool Papyrus::AddKeywordToForm(RE::StaticFunctionTag*, RE::TESForm* form, RE::BGSKeyword* keyword)
{
	if (!form || !keyword) {
		return false;
	}
	
	return Utilities::Keywords::AddKeyword(form, keyword);
}

bool Papyrus::RemoveKeywordFromForm(RE::StaticFunctionTag*, RE::TESForm* form, RE::BGSKeyword* keyword)
{
	if (!form || !keyword) {
		return false;
	}

	return Utilities::Keywords::RemoveKeyword(form, keyword);
}

float Papyrus::GenerateRandomFloat(RE::StaticFunctionTag*, float min, float max)
{
	return Utilities::GenerateRandomFloat(min, max);
}

void Papyrus::DumpArousalData(RE::StaticFunctionTag*)
{
	Debug::DumpAllArousalData();
}

void Papyrus::ClearSecondaryArousalData(RE::StaticFunctionTag*)
{
	Debug::ClearSecondaryArousalData();
}

void Papyrus::ClearAllArousalData(RE::StaticFunctionTag*)
{
	Debug::ClearAllArousalData();
}

bool Papyrus::RegisterFunctions(RE::BSScript::IVirtualMachine* vm)
{
	//OSLAroused Settings
	vm->RegisterFunction("UpdatePlayerNudityCheck", "OSLArousedNative", UpdatePlayerNudityCheck);
	vm->RegisterFunction("UpdateHourlyNudityArousalModifier", "OSLArousedNative", UpdateHourlyNudityArousalModifier);
	vm->RegisterFunction("UpdateDefaultArousalMultiplier", "OSLArousedNative", UpdateDefaultArousalMultiplier);
	
	//General State
	vm->RegisterFunction("GetArousal", "OSLArousedNative", GetArousal);
	vm->RegisterFunction("GetArousalMultiple", "OSLArousedNative", GetArousalMultiple);
	vm->RegisterFunction("GetArousalNoSideEffects", "OSLArousedNative", GetArousalNoSideEffects);

	vm->RegisterFunction("SetArousal", "OSLArousedNative", SetArousal);
	vm->RegisterFunction("SetArousalMultiple", "OSLArousedNative", SetArousalMultiple);
	vm->RegisterFunction("ModifyArousal", "OSLArousedNative", ModifyArousal);
	vm->RegisterFunction("ModifyArousalMultiple", "OSLArousedNative", ModifyArousalMultiple);

	vm->RegisterFunction("SetArousalMultiplier", "OSLArousedNative", SetArousalMultiplier);
	vm->RegisterFunction("GetArousalMultiplier", "OSLArousedNative", GetArousalMultiplier);
	vm->RegisterFunction("ModifyArousalMultiplier", "OSLArousedNative", ModifyArousalMultiplier);
	
	vm->RegisterFunction("GetExposure", "OSLArousedNative", GetExposure);
	vm->RegisterFunction("GetDaysSinceLastOrgasm", "OSLArousedNative", GetDaysSinceLastOrgasm);

	vm->RegisterFunction("SetTimeRate", "OSLArousedNative", SetTimeRate);
	vm->RegisterFunction("GetTimeRate", "OSLArousedNative", GetTimeRate);

	//Actor State
	vm->RegisterFunction("IsActorNaked", "OSLArousedNative", IsActorNaked);

	vm->RegisterFunction("RegisterSceneStart", "OSLArousedNative", RegisterSceneStart);
	vm->RegisterFunction("RemoveScene", "OSLArousedNative", RemoveScene);

	vm->RegisterFunction("RegisterActorOrgasm", "OSLArousedNative", RegisterActorOrgasm);

	//Keyword
	vm->RegisterFunction("AddKeywordToForm", "OSLArousedNative", AddKeywordToForm);
	vm->RegisterFunction("RemoveKeywordFromForm", "OSLArousedNative", RemoveKeywordFromForm);

	//Utilities
	vm->RegisterFunction("GenerateRandomFloat", "OSLArousedNative", GenerateRandomFloat);
	
	//Debug
	vm->RegisterFunction("DumpArousalData", "OSLArousedNative", DumpArousalData);
	vm->RegisterFunction("ClearSecondaryArousalData", "OSLArousedNative", ClearSecondaryArousalData);
	vm->RegisterFunction("ClearAllArousalData", "OSLArousedNative", ClearAllArousalData);

	return true;
}

void SendModEvent(RE::BSFixedString eventName, float numArg, RE::TESForm* sender)
{
	SKSE::ModCallbackEvent modEvent{
		eventName,
		RE::BSFixedString(),
		numArg,
		sender
	};

	auto modCallback = SKSE::GetModCallbackEventSource();
	modCallback->SendEvent(&modEvent);
}

void Papyrus::Events::SendActorArousalUpdatedEvent(RE::Actor* actorRef, float newVal)
{
	SendModEvent("OSLA_ActorArousalUpdated", newVal, actorRef);
}

void Papyrus::Events::SendActorNakedUpdatedEvent(RE::Actor* actorRef, bool newNaked)
{
	SendModEvent("OSLA_ActorNakedUpdated", newNaked ? 1.f : 0.f, actorRef);
}
