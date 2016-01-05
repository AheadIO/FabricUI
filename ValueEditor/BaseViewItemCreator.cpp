#include "stdafx.h"
#include "BaseViewItemCreator.h"
#include "ViewItemFactory.h"

BaseViewItemCreator::BaseViewItemCreator()
{
	ViewItemFactory* factory = ViewItemFactory::GetInstance();
	if ( !!factory )
		factory->RegisterCreator(this);
}

BaseViewItemCreator::~BaseViewItemCreator()
{
	ViewItemFactory* factory = ViewItemFactory::GetInstance();
	if ( !!factory ) 
		factory->DeRegisterCreator(this);
}
