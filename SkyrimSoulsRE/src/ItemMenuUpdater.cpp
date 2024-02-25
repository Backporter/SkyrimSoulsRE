#include "ItemMenuUpdater.h"

namespace SkyrimSoulsRE::ItemMenuUpdater
{
	ConsoleRE::TESObjectREFR* GetTargetReference()
	{
		ConsoleRE::TESObjectREFR* targetRef = nullptr;

		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();
		if (ui->IsMenuOpen(ConsoleRE::ContainerMenu::MENU_NAME))
		{
			ConsoleRE::ContainerMenu* menu = static_cast<ConsoleRE::ContainerMenu*>(ui->GetMenu(ConsoleRE::ContainerMenu::MENU_NAME).get());

			uint32_t handle = menu->GetTargetRefHandle();
			ConsoleRE::TESObjectREFRPtr refptr = nullptr;
			ConsoleRE::TESObjectREFR* ref = nullptr;
			if (ConsoleRE::TESObjectREFR::LookupByHandle(handle, refptr)) 
			{
				targetRef = refptr.get();
			}
		}
		else if (ui->IsMenuOpen(ConsoleRE::BarterMenu::MENU_NAME))
		{
			ConsoleRE::BarterMenu* menu = static_cast<ConsoleRE::BarterMenu*>(ui->GetMenu(ConsoleRE::BarterMenu::MENU_NAME).get());

			uint32_t handle = menu->GetTargetRefHandle();
			ConsoleRE::TESObjectREFRPtr refptr = nullptr;
			ConsoleRE::TESObjectREFR* ref = nullptr;
			if (ConsoleRE::TESObjectREFR::LookupByHandle(handle, refptr)) {
				targetRef = refptr.get();
				//if (targetRef->formType == ConsoleRE::FormType::ActorCharacter)
				//{
				//	ConsoleRE::Actor* vendor = targetRef->As<ConsoleRE::Actor>();
				//	ConsoleRE::TESFaction* vendorFaction = vendor->vendorFaction;

				//	if (vendorFaction && vendorFaction->vendorData.merchantContainer)
				//	{
				//		targetRef = vendorFaction->vendorData.merchantContainer;
				//	}
				//}
			}
		}
		else if (ui->IsMenuOpen(ConsoleRE::GiftMenu::MENU_NAME))
		{
			ConsoleRE::GiftMenu* menu = static_cast<ConsoleRE::GiftMenu*>(ui->GetMenu(ConsoleRE::GiftMenu::MENU_NAME).get());

			uint32_t handle = menu->GetTargetRefHandle();
			ConsoleRE::TESObjectREFRPtr refptr = nullptr;
			ConsoleRE::TESObjectREFR* ref = nullptr;
			if (ConsoleRE::TESObjectREFR::LookupByHandle(handle, refptr)) {
				targetRef = refptr.get();
			}
		}

		return targetRef;
	}

	void RequestItemListUpdate(ConsoleRE::TESObjectREFR* a_ref, ConsoleRE::TESForm* a_unk)
	{
		using func_t = decltype(&RequestItemListUpdate);
		REL::Relocation<func_t> func(Offsets::ItemMenuUpdater::RequestItemListUpdate);
		return func(a_ref, a_unk);
	}

	// Update after RemoveItem for TESObjectREFRs
	ConsoleRE::ObjectRefHandle& RemoveItem_TESObjectREFR(ConsoleRE::TESObjectREFR* a_this, ConsoleRE::ObjectRefHandle& a_handle, ConsoleRE::TESBoundObject* a_item, int32_t a_count, ConsoleRE::ITEM_REMOVE_REASON a_reason, ConsoleRE::ExtraDataList* a_extraList, ConsoleRE::TESObjectREFR* a_moveToRef, const ConsoleRE::NiPoint3* a_dropLoc = 0, const ConsoleRE::NiPoint3* a_rotate = 0)
	{
		using func_t = decltype(&RemoveItem_TESObjectREFR);
		REL::Relocation<func_t> func(Offsets::ItemMenuUpdater::RemoveItem_TESObjectREFR);
		return func(a_this, a_handle, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
	}

	// Update after RemoveItem for Actors
	ConsoleRE::ObjectRefHandle& RemoveItem_Actor(ConsoleRE::Actor* a_this, ConsoleRE::ObjectRefHandle& a_handle, ConsoleRE::TESBoundObject* a_item, int32_t a_count, ConsoleRE::ITEM_REMOVE_REASON a_reason, ConsoleRE::ExtraDataList* a_extraList, ConsoleRE::TESObjectREFR* a_moveToRef, const ConsoleRE::NiPoint3* a_dropLoc = 0, const ConsoleRE::NiPoint3* a_rotate = 0)
	{
		using func_t = decltype(&RemoveItem_Actor);
		REL::Relocation<func_t> func(Offsets::ItemMenuUpdater::RemoveItem_Actor);
		return func(a_this, a_handle, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
	}

	ConsoleRE::ObjectRefHandle& RemoveItem_Hook(ConsoleRE::TESObjectREFR* a_this, ConsoleRE::ObjectRefHandle& a_handle, ConsoleRE::TESBoundObject* a_item, int32_t a_count, ConsoleRE::ITEM_REMOVE_REASON a_reason, ConsoleRE::ExtraDataList* a_extraList, ConsoleRE::TESObjectREFR* a_moveToRef, const ConsoleRE::NiPoint3* a_dropLoc = 0, const ConsoleRE::NiPoint3* a_rotate = 0)
	{
		ConsoleRE::ObjectRefHandle result;
		if (a_this->formType == static_cast<uint32_t>(ConsoleRE::FormType::ActorCharacter))
		{
			RemoveItem_Actor(static_cast<ConsoleRE::Actor*>(a_this), a_handle, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
		}
		else
		{
			RemoveItem_TESObjectREFR(a_this, a_handle, a_item, a_count, a_reason, a_extraList, a_moveToRef, a_dropLoc, a_rotate);
		}

		ConsoleRE::TESObjectREFR* targetRef = GetTargetReference();

		if (a_this == ConsoleRE::PlayerCharacter::GetSingleton() || a_this == targetRef)
		{
			RequestItemListUpdate(a_this, nullptr);
		}

		return a_handle;
	}

	// Update after RemoveAllItems
	void RemoveAllItems_Hook(void* a_unk1, ConsoleRE::TESObjectREFR* a_containerRef, void* a_unk3, uint64_t a_unk4, uint32_t a_unk5, void* a_unk6, void* a_unk7)
	{
		using func_t = decltype(&RemoveAllItems_Hook);
		REL::Relocation<func_t> func(Offsets::ItemMenuUpdater::RemoveAllItems);
		func(a_unk1, a_containerRef, a_unk3, a_unk4, a_unk5, a_unk6, a_unk7);

		ConsoleRE::PlayerCharacter* player = ConsoleRE::PlayerCharacter::GetSingleton();
		ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

		ConsoleRE::TESObjectREFR* targetRef = GetTargetReference();

		if (a_containerRef == player || a_containerRef == targetRef)
		{
			// Some items might still remain in the list. Updating twice in a row seems to fix it for some reason.
			RequestItemListUpdate(a_containerRef, nullptr);
			RequestItemListUpdate(a_containerRef, nullptr);
		}
	}
	
#include "../../../OrbisUtil/Third-Party/herumi/xbayk/6.00/xbyak.h"

	void InstallHook()
	{
		API::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook1.address() + 0x17, (uintptr_t)RemoveAllItems_Hook);
		API::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook2.address() + 0x39, (uintptr_t)RemoveAllItems_Hook);
		API::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook3.address() + 0xB2, (uintptr_t)RemoveAllItems_Hook);
		API::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook4.address() + 0xE8, (uintptr_t)RemoveAllItems_Hook);
		API::GetTrampoline().write_call<5>(Offsets::ItemMenuUpdater::RemoveAllItems_Hook5.address() + 0x42, (uintptr_t)RemoveAllItems_Hook);

		// API::GetTrampoline().write_call<6>(Offsets::ItemMenuUpdater::RemoveItem_Hook1.address() + 0x9A, (uintptr_t)RemoveItem_Hook);
		API::GetTrampoline().write_call<6>(Offsets::ItemMenuUpdater::RemoveItem_Hook2.address() + 0xF7, (uintptr_t)RemoveItem_Hook);
		API::GetTrampoline().write_call<6>(Offsets::ItemMenuUpdater::RemoveItem_Hook3.address() + 0x48C, (uintptr_t)RemoveItem_Hook);
		
#if true
		// the PS4 build doesn't do a 5/6 byte call, instead does a two byte call on rax, so we'll just replace rax with our function pointer.
		struct DUMMY : Xbyak::CodeGenerator
		{
			DUMMY()
			{
				mov(rax, uintptr_t(RemoveItem_Hook));
			}
		};

		//
		auto& localTrampoline = API::GetTrampoline();

		//
		DUMMY xbaykcode;

		//
		auto buf = localTrampoline.allocate(xbaykcode);
		localTrampoline.write_branch<6>(Offsets::ItemMenuUpdater::RemoveItem_Hook4.address() + 0x12A, buf);

		REL::safe_write(Offsets::ItemMenuUpdater::RemoveItem_Hook4.address() + 0x12A + 6, 0x90);
#else
		API::GetTrampoline().write_call<6>(Offsets::ItemMenuUpdater::RemoveItem_Hook4.address() + 0x268, (uintptr_t)RemoveItem_Hook);
#endif
	}
}
