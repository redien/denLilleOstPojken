#include "MenuState.h"
#include "StateManager.h"

#include "DebugPrint.h"
#include "ToString.h"

//=========================================================================
// Private Functions
//=========================================================================
void MenuState::resetItemOffset( int p_idx )
{
	if(m_currMenu < m_menus.size() && p_idx < m_menus[m_currMenu]->m_items.size())
		m_menus[m_currMenu]->m_items[p_idx]->setTextOffset(0.0f, 0.0f);
}

void MenuState::nextItem()
{
	if( m_currItemIdx == m_menus[m_currMenu]->m_items.size() -1 )
		m_currItemIdx = 0;
	else
		m_currItemIdx++;

	playSound();
}

void MenuState::prevItem()
{
	if( m_currItemIdx == 0 )
		m_currItemIdx =  m_menus[m_currMenu]->m_items.size()-1;
	else
		m_currItemIdx--;

	playSound();
}

void MenuState::setCurrMenu( int p_menu )
{
	m_menus[m_currMenu]->deActivate();
	m_currMenu = p_menu;
	m_menus[m_currMenu]->activate();
}

void MenuState::selectMmItem()
{
	switch(m_currItemIdx)
	{
	case MenuSubState::MM_LEVEL_SELECT:
		setCurrMenu(MenuSubState::MENU_LEVEL_SELECT); break;
	case MenuSubState::MM_HIGHSCORE:
		setCurrMenu(MenuSubState::MENU_HIGHSCORE); break;
	case MenuSubState::MM_CREDITS:
		setCurrMenu(MenuSubState::MENU_CREDITS); break;
	case MenuSubState::MM_EXIT:
		setCurrMenu(MenuSubState::MENU_EXIT); break;
	default:
		break;
	}
}


void MenuState::selectLsItem()
{
	switch(m_currItemIdx)
	{
	case MenuSubState::LS_MAIN:
		setCurrMenu(MenuSubState::MENU_MAIN);
		break;
	case MenuSubState::LS_START_LEVEL:
		m_parent->requestStateChange(m_parent->getInGameState());
		break;
	default:
		break;
	}
}


void MenuState::selectExState()
{
	switch(m_currItemIdx)
	{
	case MenuSubState::EX_YES:
		m_parent->terminate();
		break;
	case MenuSubState::EX_NO:
		setCurrMenu(MenuSubState::MENU_MAIN);
		break;
	default:
		break;
	}
}

bool MenuState::playSound()
{
	if( m_itemSelectSnd != NULL )
	{
		m_itemSelectSnd->play = true;
		return true;
	}
	else
	{
		return false;
	}
}

void MenuState::initMenuItems()
{
	float fw = 1.0f/1920.0f;
	float fh = 1.0f/1080.0f;
	m_bgItem = m_factory->createMenuItem( 
		fVector3( 0.5f, 0.5f, 0.5f ), fVector2( 1.0f, 1.0f ),
		"COPYRIGHT 2012 MAJESTIC 12", fVector2(0.0f, -fh*500.0f),
		fVector2(fw*32, fh*32), "../Textures/SplashScreen.png" );

	m_menus.push_back(new MenuSubState( MenuSubState::MENU_MAIN, m_factory));
	m_menus.push_back(new MenuSubState( MenuSubState::MENU_LEVEL_SELECT, m_factory));
	m_menus.push_back(new MenuSubState( MenuSubState::MENU_HIGHSCORE, m_factory));
	m_menus.push_back(new MenuSubState( MenuSubState::MENU_CREDITS, m_factory));
	m_menus.push_back(new MenuSubState( MenuSubState::MENU_EXIT, m_factory));

	for( unsigned int i=0; i<m_menus.size(); i++)
	{
		m_menus[i]->deActivate();
	}

	m_currMenu = 0;
	m_menus[m_currMenu]->activate();

	m_itemSelectSnd = m_factory->CreateSoundInfo( "../Sounds/Plink_08.wav", 80 );
}

//=========================================================================
// Public Functions
//=========================================================================
MenuState::MenuState(StateManager* p_parent, IODevice* p_io, vector<MapData> p_maps): State(p_parent)
{
	m_maps = p_maps;
	m_io = p_io;
	m_factory = NULL;
	m_itemSelectSnd = NULL;
	m_currItemIdx = 0;
	m_totTime = 0.0f;
	m_currMenu = MenuSubState::MENU_MAIN;

}

MenuState::~MenuState()
{
	onExit();
}

bool MenuState::onEntry()
{
	if (!m_resourcesAllocated) // only allocate once
	{
		if (m_io)
		{
			m_factory = new GOFactory(m_io);
			m_io->clearSpriteInfos();
			if (m_factory)
				initMenuItems();
		}
		m_resourcesAllocated = true;
	}
	return true;
}

bool MenuState::onExit()
{
	if (m_resourcesAllocated) // only deallocate if allocated
	{
		if (m_io)
		{
			delete m_bgItem;
			for( unsigned int menuIdx=0; menuIdx<m_menus.size(); menuIdx++ )
				delete m_menus[menuIdx];
			m_menus.clear();
			delete m_factory;		
			m_io->clearSpriteInfos();
		}
		m_resourcesAllocated=false;
	}
	return true;
}

void MenuState::update(float p_dt)
{
	m_totTime +=  p_dt;

	float fac = sin(m_totTime*5)*3;
	float fac2 = cos(m_totTime*5)*3;

	if(m_currMenu < m_menus.size() && m_currItemIdx < m_menus[m_currMenu]->m_items.size())
		m_menus[m_currMenu]->m_items[m_currItemIdx]->setTextOffset(fac2, fac);

	if (m_io)
	{
		InputInfo input = m_io->fetchInput();

		// NOTE: The way this function works right now
		// (triggers state change and sprite dealloc), 
		// it has to the last function called in update.
		handleInput(input);
	}
	//m_menus[m_currMenu]->activate();

}
void MenuState::draw(float p_dt)
{
}

void MenuState::handleInput(InputInfo p_input)
{
	if( p_input.keys[InputInfo::UP] == InputInfo::KEYPRESSED ||
		p_input.keys[InputInfo::W_KEY] == InputInfo::KEYPRESSED )
		prevItem();

	else if ( p_input.keys[InputInfo::DOWN] == InputInfo::KEYPRESSED ||
		p_input.keys[InputInfo::S_KEY] == InputInfo::KEYPRESSED )
		nextItem();

	if ( p_input.keys[InputInfo::ENTER] == InputInfo::KEYPRESSED ||
		p_input.keys[InputInfo::SPACE] == InputInfo::KEYPRESSED )
	{
		m_menus[m_currMenu]->deActivate();

		switch(m_currMenu)
		{
		case MenuSubState::MENU_MAIN:
			selectMmItem();
			break;
		case MenuSubState::MENU_LEVEL_SELECT:
			selectLsItem();
			break;
		case MenuSubState::MENU_EXIT:
			selectExState();
			break;
		default:
			break;
		}
		
		m_menus[m_currMenu]->activate();

		m_currItemIdx = 0;
	}

	else if( p_input.keys[InputInfo::ESC] == InputInfo::KEYPRESSED || !m_io->isRunning() )
		switch(m_currMenu)
		{
			// Go to exit menu if in main
		case MenuSubState::MENU_MAIN:
			setCurrMenu(MenuSubState::MENU_EXIT);
			break;
			
			// Else go to main
		default:
			setCurrMenu(MenuSubState::MENU_MAIN);
	}
}