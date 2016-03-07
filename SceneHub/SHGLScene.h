/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_SCENE_H__
#define __UI_SCENEHUB_SCENE_H__

#include <QtGui/QMenu>
#include <QtGui/QDrag>
#include <QtGui/QTreeView>
#include <QtGui/QMouseEvent>
#include <FabricCore.h>
#include <FTL/JSONEnc.h>
#include <FTL/OwnedPtr.h>
#include <FTL/JSONValue.h>

namespace FabricUI
{
  namespace SceneHub
  {
    class SHGLScene {

      public:
        SHGLScene(FabricCore::Client client, QString sceneName);

        SHGLScene(FabricCore::Client client, FabricCore::RTVal shGLScene);

        ~SHGLScene() {}
        
        /// Gets the client.
        FabricCore::Client getClient() { return m_client; };

        /// Gets a reference to the scenegraph.
        bool getSHGLScene(FabricCore::RTVal &shGLScene);

        /// Gets a reference to the scenegraph.
        FabricCore::RTVal getSG();

        /// Gets a reference to the scenegraph.
        FabricCore::RTVal getSceneRoot();

        /// Gets a reference to the scenegraph.
        FabricCore::RTVal getMaterialLibraryRoot();

        /// Gets a reference to the scenegraph.
        FabricCore::RTVal getImageLibraryRoot();

        /// Gets a reference to the scenegraph.
        FabricCore::RTVal getAssetLibraryRoot();

        /// Gets the drawing statistics.
        void getSceneStats(uint32_t &meshCount, uint32_t &triangleCount, uint32_t &lightCount);
        
        /// Prepares for rendering. 
        /// This must be called once before re-drawing all viewports.
        void prepareSceneForRender();

        /// Gets the initiale   time-line state, for playback.
        /// \param enable Is the playback active.
        /// \param startFrame The timeline first frame.
        /// \param endFrame The timeline last frame.
        /// \param fps The number of frames per second.
        void getInitialTimelineState(
          bool &enable, 
          int &startFrame, 
          int &endFrame, 
          float &fps);

        /// Activates the playback at app opening.
        bool playbackByDefault();
        
        /// Sets the current frame, for animation.
        /// \param frame The frame number
        void setFrame(int frame);

        /// Returns true if the structure of the SceneGraph changed since the last call to this function.
        /// This allows the C++ app to filter multiple consecutive "sceneChanged" notifications.
        bool sceneHierarchyChanged(); 

        /// Returns an array of SGObject that are currently in the 'selection' set. 
        /// This is used for reflecting the 3D selection to the TreeView (which might or not have these as UI nodes).
        FabricCore::RTVal getSelectedObjects(); 

        /// Updates the tree-view when selection from the scene.
        /// \param obj The selected scenegraph object
        void sceneItemSelected(FabricCore::RTVal obj); 

        /// Updates the 'selection' from TreeView's selection.
        /// \param obj The selected scenegraph object
        void treeItemSelected(FabricCore::RTVal obj); 

        /// Updates the 'selection' from TreeView's selection.
        /// \param obj The selected scenegraph object
        void treeItemDeselected(FabricCore::RTVal obj); 

        bool showTreeViewByDefault(uint32_t &level);
        
        /// Adds texture or asset files.
        /// Contains a list of file paths associated with a mouse position. 
        /// Depending on their type, a Scene hierarchy or a texture might be created.
        /// \param pathList The array of pathes to read
        /// \param pos The scene position, use to asset/texture placement
        /// \param forceExpand Force expension
        void addExternalFileList(QStringList pathList, float pos[3], bool forceExpand);

        /// Updates selected intance(s) with provided properties
        /// \param color The instance color
        /// \param local Local proprety if true
        void setObjectColor(float color[4], bool local);

        /// Adding lights
        /// \param lightType The type of light (spot, point, diectional...)
        void addLight(uint32_t lightType, float pos[3]);

        /// Updates selected light(s) with provided properties
        /// \param color The light color
        /// \param intensisty The light intensisty
        void setlightProperties(float color[4], float intensity);

        /// Exports the scene to alembic.
        /// \param filePath The path of the alembic file.
        void exportToAlembic(QString filePath);
                

        /// Gets the command manager.
        bool getCmdManager(FabricCore::RTVal &cmdManager);

        /// From the parameter type and its value, creates 
        /// \param type The parameter type
        /// \param value The parameter value JSon encoded
        FabricCore::RTVal setParamValue(std::string type, std::string value);
        
        /// Gets the command at index i of KL stack.
        /// \param index The indo of the commands in the stack.
        FabricCore::RTVal retrieveCmd(uint32_t index);

        /// Gets the number of commands in the kl stacks.
        /// Uses for synchronizaztion
        uint32_t SHGLScene::getNumCmdInUndoStack();

        /// Executes the command at index i of KL stack.
        /// \param cmdName The name of the commands
        /// \param params The paramters
        void executeCmd(std::string cmdName, std::vector<FabricCore::RTVal> &params);

        /// Undo commands.
        /// \param undoCount The number of undos. 
        void undoCmd(uint32_t undoCount);

        /// Redo commands.
        /// \param redoCount The number of redos. 
        void redoCmd(uint32_t redoCount);

        
      private:
        /// Encodes a rtVal into a Json, saves the rtVal
        /// \param client The core client
        /// \param rtVal The value to encode
        std::string EncodeRTValToJSON(FabricCore::Client client, FabricCore::RTVal rtVal);

        /// Decodes a rtVal from a Json, reads the rtVal
        /// \param client The core client
        /// \param rtVal The result value
        /// \param json The string to decode
        void DecodeRTValFromJSON(FabricCore::Client client, FabricCore::RTVal &rtVal, FTL::CStrRef json); 
      
      
        /// \internal
        FabricCore::Client m_client;    
        /// \internal
        FabricCore::RTVal m_shGLScene;
        // \internal
        //FabricCore::RTVal m_shGLSceneDefault;
    };

  }
}

#endif // __UI_SCENEHUB_SCENE_H__
