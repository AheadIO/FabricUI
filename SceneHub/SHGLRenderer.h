/*
 *  Copyright 2010-2016 Fabric Software Inc. All rights reserved.
 */

#ifndef __UI_SCENEHUB_GLRENDERER_H__
#define __UI_SCENEHUB_GLRENDERER_H__

#include <QtGui/QMenu>
#include <QtGui/QDrag>
#include <QtGui/QTreeView>
#include <QtGui/QMouseEvent>
#include <FabricCore.h>
 
namespace FabricUI
{
  namespace SceneHub
  {
    class SHGLRenderer {

      public:
        SHGLRenderer(FabricCore::Client *client) : m_client(client) {}

        ~SHGLRenderer() {}
        
        /// Gets the drawing statistics.
        /// \param viewportID The viewport ID.
        /// \param obj the total number of renderer objects
        /// \param point the total number of renderer points
        /// \param line the total number of renderer lines
        /// \param triangle the total number of renderer triangless
        void SHGLRenderer::getDrawStats(
          uint32_t viewportID, 
          uint32_t &obj, 
          uint32_t &point, 
          uint32_t &line, 
          uint32_t &triangle);
        
        /// Fast getter to the viewport at this viewportID.
        /// \param viewportID The viewport ID.
        FabricCore::RTVal getViewport(uint32_t viewportID);
        
        /// Gets/creates a viewport.
        /// \param viewportID The viewport ID.
        FabricCore::RTVal getOrAddViewport(uint32_t viewportID);
        
        /// Gets/creates a setreo-viewport.
        /// \param viewportID The viewport ID.
        FabricCore::RTVal getOrAddStereoViewport(uint32_t viewportID);
        
        /// Removes the viewport.
        /// \param viewportID The viewport ID. 
        void removeViewport(uint32_t viewportID);
        
        /// Sets the viewport camera as orthographic.
        /// \param viewportID The viewport ID.
        void setOrthographicViewport(uint32_t viewportID, bool orthographic);
        
        /// Gets the viewport camera.
        /// \param viewportID The viewport ID.
        FabricCore::RTVal getCamera(uint32_t viewportID);
        
        /// Casts a ray from a Viewport and a position in camera space.
        /// \param viewportID The ID of the viewport.
        /// \param pos The mouse's position in the viewport
        FabricCore::RTVal castRay(uint32_t viewportID, float pos[2]);

        /// This helper method will raycast in the scene. 
        // If no result is found, it will intersect with the Y = 0 plane. 
        // If this is outside the near/far camera range, it will return the origin.
        // \param viewportID The ID of the viewport.
        // \param pos The mouse's position in the viewport
        void get3DScenePosFrom2DScreenPos(uint32_t viewportID, float pos[2], float *pos3D);

        /// Activates the playback.
        /// \param playback It true, playback.
        void setPlayback(bool playback);
        
        /// Checks if the playback is active.
        bool isPlayback(bool playback);
        
        /// Renders within this viewport.
        /// \param viewportID The viewport ID.
        /// \param width the viewport width.
        /// \param height the viewport height.
        /// \param samples Anti-aliasing number of samples.
        void render(
          uint32_t viewportID,
          uint32_t width,
          uint32_t height,
          uint32_t samples);
        
        /// Renders within this viewport.
        /// \param viewportID The viewport ID.
        /// \param width the viewport width.
        /// \param height the viewport height.
        /// \param samples Anti-aliasing number of samples.
        /// \param drawPhase Drawing phases (pre-post draw).
        void render(
          uint32_t viewportID,
          uint32_t width,
          uint32_t height,
          uint32_t samples,
          uint32_t drawPhase);
        
        /// Propagates the events.
        /// \param event The event.
        bool onEvent(uint32_t viewportID, QEvent *event, bool &redrawAllViewports);
      
        /// Gets the names and the keys of the registered tools.
        /// \param toolNames Name of the registered tools.
        /// \param enableKeys Key of the registered tools.
        void getRegisteredTools(QStringList &toolNames, QStringList &toolKeys);
        
        /// Returns the selection set if any.
        FabricCore::RTVal getSelectionSet();
        
        /// Checks if the selection changed from the manipulation system.
        /// Synchronizes with the tree-view.
        bool selectionChangedFromManips();
        
        /// Gets the category of the selection.
        /// Used to know what type of element is selected.
        /// For showing the right contextual menu.
        QString getSelectionCategory();


      private:
        /// \internal
        FabricCore::Client *m_client;    
        /// \internal
        FabricCore::RTVal getSHGLRenderer();
    };
  }
}

#endif // __UI_SCENEHUB_GLRENDERER_H__
