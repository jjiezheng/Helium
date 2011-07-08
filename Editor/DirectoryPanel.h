#pragma once

#include "SceneGraph/SceneManager.h"

#include "Editor/EditorGenerated.h"
#include "Editor/HierarchyOutliner.h"
#include "Editor/TreeMonitor.h"

namespace Helium
{
    namespace Editor
    {
        class DirectoryPanel : public DirectoryPanelGenerated
        {
        public:
            DirectoryPanel( SceneGraph::SceneManager* manager, TreeMonitor* treeMonitor, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
            virtual ~DirectoryPanel();

            void SaveState( SceneOutlinerState& hierarchyState, SceneOutlinerState& entityState, SceneOutlinerState& typesState );
            void RestoreState( SceneOutlinerState& hierarchyState, SceneOutlinerState& entityState, SceneOutlinerState& typesState );

        private:
            SceneGraph::SceneManager*	m_SceneManager;
            TreeMonitor*				m_TreeMonitor;
            HierarchyOutliner*			m_HierarchyOutline;
        };
    }
}