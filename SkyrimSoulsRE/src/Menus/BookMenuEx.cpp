#include "Menus/BookMenuEx.h"

namespace SkyrimSoulsRE
{
	void BookMenuEx::AdvanceMovie_Hook(float a_interval, uint32_t a_currentTime)
	{
		class AdvanceBookMenuTask : public UnpausedTask
		{
			float interval;
			uint32_t currentTime;

		public:

			AdvanceBookMenuTask(float a_interval, uint32_t a_currentTime)
			{
				this->interval = a_interval;
				this->currentTime = a_currentTime;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::BookMenu::MENU_NAME))
				{
					ConsoleRE::BookMenu* menu = static_cast<ConsoleRE::BookMenu*>(ui->GetMenu(ConsoleRE::BookMenu::MENU_NAME).get());

					static_cast<BookMenuEx*>(menu)->_AdvanceMovie(menu, interval, currentTime);
				}
			}
		};

		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();
		queue->AddTask(new AdvanceBookMenuTask(a_interval, a_currentTime));

		AutoCloseManager::GetSingleton()->CheckAutoClose(ConsoleRE::BookMenu::MENU_NAME);
	}

	ConsoleRE::UI_MESSAGE_RESULTS BookMenuEx::ProcessMessage_Hook(ConsoleRE::UIMessage& a_message)
	{
		class ProcessMessageTask : public UnpausedTask
		{
			ConsoleRE::UIMessage* message;
		public:
			ProcessMessageTask(ConsoleRE::UIMessage* a_message)
			{
				this->message = a_message;
				this->usesDelay = false;
			}

			void Run() override
			{
				ConsoleRE::UI* ui = ConsoleRE::UI::GetSingleton();

				if (ui->IsMenuOpen(ConsoleRE::BookMenu::MENU_NAME))
				{
					BookMenuEx* menu = static_cast<BookMenuEx*>(ui->GetMenu(ConsoleRE::BookMenu::MENU_NAME).get());
					menu->_ProcessMessage(menu, *message);
				}
			}

			void Dispose() override
			{
				delete this->message;
				delete this;
			}
		};

		ConsoleRE::UserEvents* userEvents = ConsoleRE::UserEvents::GetSingleton();
		UnpausedTaskQueue* queue = UnpausedTaskQueue::GetSingleton();

		switch (a_message.type)
		{
			case ConsoleRE::UI_MESSAGE_TYPE::kUserEvent:
			{
				ConsoleRE::BSUIMessageData* oldData = static_cast<ConsoleRE::BSUIMessageData*>(a_message.data);

				if (oldData->fixedStr == userEvents->leftEquip || oldData->fixedStr == userEvents->rightEquip ||
					oldData->fixedStr == userEvents->prevPage || oldData->fixedStr == userEvents->nextPage ||
					oldData->fixedStr == userEvents->accept || oldData->fixedStr == userEvents->cancel)
				{
					ConsoleRE::UIMessageQueue* uiMessageQueue = ConsoleRE::UIMessageQueue::GetSingleton();
					ConsoleRE::InterfaceStrings* interfaceStrings = ConsoleRE::InterfaceStrings::GetSingleton();

					ConsoleRE::UIMessage* message = new ConsoleRE::UIMessage(a_message);
					ConsoleRE::MessageDataFactoryManager* msgFactory = ConsoleRE::MessageDataFactoryManager::GetSingleton();
					auto creator = msgFactory->GetCreator(interfaceStrings->bsUIMessageData);
					ConsoleRE::BSUIMessageData* data = static_cast<ConsoleRE::BSUIMessageData*>(creator->Create());

					data->data = oldData->data;
					data->str = oldData->str;
					data->fixedStr = oldData->fixedStr;

					message->data = data;

					ProcessMessageTask* task = new ProcessMessageTask(message);
					queue->AddTask(task);
					return ConsoleRE::UI_MESSAGE_RESULTS::kHandled;
				}
				return ConsoleRE::UI_MESSAGE_RESULTS::kIgnore;
			}
			case ConsoleRE::UI_MESSAGE_TYPE::kUpdate:
			{
				ConsoleRE::UIMessage* message = new ConsoleRE::UIMessage(a_message);
				message->data = nullptr;

				ProcessMessageTask* task = new ProcessMessageTask(message);
				queue->AddTask(task);
				return ConsoleRE::UI_MESSAGE_RESULTS::kHandled;
			}
			case ConsoleRE::UI_MESSAGE_TYPE::kShow:
			{
				ConsoleRE::UIMessage* message = new ConsoleRE::UIMessage(a_message);
				message->data = nullptr;

				ProcessMessageTask* task = new ProcessMessageTask(message);
				queue->AddTask(task);
				return ConsoleRE::UI_MESSAGE_RESULTS::kHandled;
			}
			case ConsoleRE::UI_MESSAGE_TYPE::kHide:
			{
				ConsoleRE::UIMessage* message = new ConsoleRE::UIMessage(a_message);
				message->data = nullptr;

				ProcessMessageTask* task = new ProcessMessageTask(message);
				queue->AddTask(task);

				if (this->closeMenu)
				{
					// close animation done
					return ConsoleRE::UI_MESSAGE_RESULTS::kHandled;
				}
				else
				{
					// close animation still playing
					return ConsoleRE::UI_MESSAGE_RESULTS::kIgnore;
				}
			}
		}
		return _ProcessMessage(this, a_message);
	}

	ConsoleRE::IMenu* BookMenuEx::Creator()
	{
		ConsoleRE::BookMenu* menu = static_cast<ConsoleRE::BookMenu*>(CreateMenu(ConsoleRE::BookMenu::MENU_NAME));

		AutoCloseManager* autoCloseManager = AutoCloseManager::GetSingleton();
		autoCloseManager->InitAutoClose(ConsoleRE::BookMenu::MENU_NAME, menu->GetTargetReference(), false);

		return menu;
	}

	void BookMenuEx::InstallHook()
	{
		//Hook AdvanceMovie
		REL::Relocation<uintptr_t> vTable(Offsets::Menus::BookMenu::Vtbl);
		_ProcessMessage = vTable.write_vfunc(0x5, &BookMenuEx::ProcessMessage_Hook);
		_AdvanceMovie = vTable.write_vfunc(0x6, &BookMenuEx::AdvanceMovie_Hook);
	}
}
