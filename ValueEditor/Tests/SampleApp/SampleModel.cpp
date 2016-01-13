#include "StdAfx.h"
#include "../../QVariantRTVal.h"
#include "SampleModel.h"

BaseModelItem* BuildSampleModel( FabricCore::Client& client )
{
	SampleModelItem* rootItem = new SampleModelItem("RootNode", QVariant());

	// Our node has a color metadata, display it here
	QVariant colorVar = QVariant::fromValue(QColor(128, 20, 50));
	SampleModelItem* metaColor = new SampleModelItem("RootNodeColor", colorVar);
	rootItem->AddChild(metaColor);


	// Build a simple hierarchy that loosely resembles a few ports
	QVariant fltvar = QVariant(1.0f);
	SampleModelItem* floatPort = new SampleModelItem("myFloat", fltvar);
	rootItem->AddChild(floatPort);

  // Build a simple hierarchy that loosely resembles a few ports
  QVariant intvar = QVariant( 6 );
  SampleModelItem* intPortRanged = new SampleModelItem( "myIntRanged", intvar );
  ViewItemMetadata* intMeta = static_cast<ViewItemMetadata*>
    (intPortRanged->GetMetadata());
  intMeta->setSInt32( "min", -13 );
  intMeta->setSInt32( "max", -27 );
  rootItem->AddChild( intPortRanged );

	QVariant vec3Var = QVector3D(1.3f, 2.5f, 0.1f);
	SampleModelItem* vec3Port = new SampleModelItem("myVec", vec3Var);
	rootItem->AddChild(vec3Port);

	// Make a sub-group
	SampleModelItem* pSubGroup = new SampleModelItem("SubGroup", QVariant());
	rootItem->AddChild(pSubGroup);


	SampleModelItem* diffFloat = new SampleModelItem("diffFloat", QVariant(7.9f));
	pSubGroup->AddChild(diffFloat);

	colorVar = QVariant::fromValue(QColor::fromHsvF(0.3, 0, 0.85));
	SampleModelItem* subColor = new SampleModelItem("aColour", colorVar);
	pSubGroup->AddChild(subColor);

  client.loadExtension( "Math", NULL, 0 );
  FabricCore::RTVal val =
    FabricCore::RTVal::Construct( client, "Xfo", 0, NULL );
  QVariant xfoVar = toVariant( val );
  SampleModelItem* subXfo = new SampleModelItem( "transform", xfoVar );
  pSubGroup->AddChild( subXfo );

	return rootItem;
}

bool SampleModelItem::hasDefault()
{
  return false;
}

void SampleModelItem::resetToDefault()
{
}
