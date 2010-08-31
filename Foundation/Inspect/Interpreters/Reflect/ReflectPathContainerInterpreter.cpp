#include "ReflectPathContainerInterpreter.h"

#ifdef INSPECT_REFACTOR
#include "Foundation/Inspect/DragDrop/FilteredDropTarget.h"
#endif

#include "Foundation/Inspect/Controls/LabelControl.h"
#include "Foundation/Inspect/Controls/ValueControl.h"
#include "Foundation/Inspect/Controls/ChoiceControl.h"
#include "Foundation/Inspect/Controls/ListControl.h"
#include "Foundation/Inspect/Controls/ButtonControl.h"
#include "Foundation/Inspect/Data.h"
#include "Foundation/Inspect/Inspect.h"
#include "Foundation/Inspect/Interpreters/Reflect/InspectReflectInit.h"

#include "Core/Asset/AssetClass.h"
#include "Foundation/String/Tokenize.h"
#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Inspect;

PathContainerInterpreter::PathContainerInterpreter (Container* labelContainer)
: ReflectFieldInterpreter (labelContainer)
, m_List( NULL )
, m_FileFilter( TXT( "" ) )
{

}

void PathContainerInterpreter::InterpretField(const Field* field, const std::vector<Reflect::Element*>& instances, Container* parent)
{
    m_List = NULL;

    if (field->m_Flags & FieldFlags::Hide)
    {
        return;
    }

    bool isArray = ( field->m_SerializerID == Reflect::GetType<PathArraySerializer>() );
    bool isSet = ( field->m_SerializerID == Reflect::GetType<PathSetSerializer>() );
    bool isContainer = isArray || isSet;

    // create the label
    ContainerPtr labelContainer = CreateControl< Container >();
    parent->AddChild( labelContainer );

    LabelPtr label = CreateControl< Label >();
    labelContainer->AddChild( label );
    label->BindText( field->m_UIName );

    // create the list view
    ContainerPtr listContainer = new Container ();
    parent->AddChild( listContainer );

    ListPtr list = CreateControl< List >();
    m_List = list;
    listContainer->AddChild( list );

    // create the buttons
    ButtonPtr editButton;
    ButtonPtr findButton;
    ButtonPtr addButton;
    ButtonPtr removeButton;
    ButtonPtr upButton;
    ButtonPtr downButton;
    if ( !(field->m_Flags & FieldFlags::ReadOnly) )
    {
        addButton = CreateControl< Button >();
        if ( isContainer )
        {
            tstring filter;
            field->GetProperty( TXT("FileFilter"), filter );

            // Add button - normal file open dialog
            addButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnAddFile ) );
#ifdef INSPECT_REFACTOR
            addButton->SetClientData( new ClientDataFilter( list, instances.front()->GetType(), filter ) );
#endif
            // Add button - opens file browser
            findButton = CreateControl< Button >();
            findButton->a_Icon.Set( TXT( "actions/system-search" ) );
            findButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnFindFile ) );
#ifdef INSPECT_REFACTOR
            findButton->SetClientData( new ClientDataFilter( list, instances.front()->GetType(), filter ) );
#endif

            // Edit button - attempt to edit the selected file
            editButton = CreateControl< Button >();
            editButton->a_Label.Set( TXT( "Edit" ) );
            editButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnEdit ) );
            editButton->SetClientData( new ClientData( list ) );
        }
        else
        {
            addButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnAdd ) );
            addButton->SetClientData( new ClientData( list ) );
        }
        addButton->a_Label.Set( TXT( "Add" ) );

        removeButton = CreateControl< Button >();
        removeButton->a_Label.Set( TXT( "Remove" ) );
        removeButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnRemove ) );
        removeButton->SetClientData( new ClientData( list ) );

        if ( isArray )
        {
            upButton = CreateControl< Button >();
            upButton->a_Icon.Set( TXT( "actions/go-up" ) );
            upButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnMoveUp ) );
            upButton->SetClientData( new ClientData( list ) );

            downButton = CreateControl< Button >();
            downButton->a_Icon.Set( TXT( "actions/go-down" ) );
            downButton->ButtonClickedEvent().Add( ButtonClickedSignature::Delegate ( this, &PathContainerInterpreter::OnMoveDown ) );
            downButton->SetClientData( new ClientData( list ) );
        }

#ifdef INSPECT_REFACTOR
        Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( m_FinderSpec );
        filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &PathContainerInterpreter::OnDrop ) );
        m_List->SetDropTarget( filteredDropTarget );
#endif
    }

    // add the buttons to the container
    ContainerPtr buttonContainer = new Container ();
    parent->AddChild( buttonContainer );
    if ( addButton )
    {
        buttonContainer->AddChild( addButton );
    }
    if ( findButton )
    {
        buttonContainer->AddChild( findButton );
    }
    if ( editButton )
    {
        buttonContainer->AddChild( editButton );
    }
    if ( removeButton )
    {
        buttonContainer->AddChild( removeButton );
    }
    if ( upButton )
    {
        buttonContainer->AddChild( upButton );
    }
    if ( downButton )
    {
        buttonContainer->AddChild( downButton );
    }

    // create the serializers
    std::vector<Reflect::Element*>::const_iterator itr = instances.begin();
    std::vector<Reflect::Element*>::const_iterator end = instances.end();
    for ( ; itr != end; ++itr )
    {
        SerializerPtr s = field->CreateSerializer();

        s->ConnectField(*itr, field);

        m_Serializers.push_back(s);
    }

    // bind the ui to the serializers
    Helium::SmartPtr< MultiStringFormatter<Serializer> > data = new MultiStringFormatter<Serializer>( (std::vector<Reflect::Serializer*>&)m_Serializers );
    list->Bind( data );
    if ( addButton && isContainer )
    {
        addButton->Bind( data );
    }

    // setup the default value
    if (field->m_Default != NULL)
    {
        tstringstream outStream;
        *field->m_Default >> outStream;
        list->a_Default.Set( outStream.str() );
    }
}

void PathContainerInterpreter::OnAdd( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );

        Path path = d_OpenFileDialog.Invoke( FileDialogArgs( Helium::FileDialogTypes::OpenFile, TXT( "Add File" ), TXT( "*.*" ) ) );
        if ( !path.empty() )
        {
            List* list = static_cast< List* >( data->GetControl() );
#ifdef INSPECT_REFACTOR
            list->AddItem( path.Get() );
#endif
        }

        args.m_Control->GetCanvas()->Read();
    }
}

void PathContainerInterpreter::OnAddFile( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();

    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataFilter>() ) )
    {
        ClientDataFilter* data = static_cast< ClientDataFilter* >( clientData.Ptr() );

        HELIUM_BREAK();
#pragma TODO( "Reimplement to use an event that interfaces with the File Dialog" )
        //if ( browserDlg.ShowModal() == wxID_OK )
        //{
        //  tstring filePath = browserDlg.GetPath().c_str();
        //  data->m_List->AddItem( filePath );
        //}

        args.m_Control->GetCanvas()->Read();
    }
}

void PathContainerInterpreter::OnFindFile( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();

    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientDataFilter>() ) )
    {
        ClientDataFilter* data = static_cast< ClientDataFilter* >( clientData.Ptr() );

        HELIUM_BREAK();
#pragma TODO( "Reimplement to use an event that interfaces with the Vault" )
        //if ( browserDlg.ShowModal() == wxID_OK )
        //{
        //  data->m_List->AddItem( browserDlg.GetPath().c_str() );
        //}

        args.m_Control->GetCanvas()->Read();
    }
}

void PathContainerInterpreter::OnEdit( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        const std::set< size_t >& selectedItemIndices = list->a_SelectedItemIndices.Get();

        // It would be nice to iterate over the selection here, but this is not safe since
        // each call to open can destroy this control and invalidate our iterator.
        if ( !selectedItemIndices.empty() )
        {
#ifdef INSPECT_REFACTOR
            // need to resolve from index to path in the data
            g_EditFilePath.Raise( EditFilePathArgs( *( selectedItemIndices.begin() ) ) );
#endif
        }
    }
}

void PathContainerInterpreter::OnRemove( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
        const std::set< size_t >& selectedItemIndices = list->a_SelectedItemIndices.Get();
        if ( !selectedItemIndices.empty() )
        {
            std::set< size_t >::const_iterator itr = selectedItemIndices.begin();
            std::set< size_t >::const_iterator end = selectedItemIndices.end();
            for ( ; itr != end; ++itr )
            {
#ifdef INSPECT_REFACTOR
                const tstring& selection = *itr;
                list->RemoveItem( selection );
#endif
            }
        }
    }
}

void PathContainerInterpreter::OnMoveUp( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
#ifdef INSPECT_REFACTOR
        list->MoveSelectedItems( Inspect::MoveDirections::Up );
#endif
    }
}

void PathContainerInterpreter::OnMoveDown( const ButtonClickedArgs& args )
{
    Reflect::ObjectPtr clientData = args.m_Control->GetClientData();
    if ( clientData.ReferencesObject() && clientData->HasType( Reflect::GetType<ClientData>() ) )
    {
        ClientData* data = static_cast< ClientData* >( clientData.Ptr() );
        List* list = static_cast< List* >( data->GetControl() );
#ifdef INSPECT_REFACTOR
        list->MoveSelectedItems( Inspect::MoveDirections::Down );
#endif
    }
}

#ifdef INSPECT_REFACTOR
void PathContainerInterpreter::OnDrop( const Inspect::FilteredDropTargetArgs& args )
{
    if ( args.m_Paths.size() )
    {
        m_List->Freeze();
        for ( std::vector< tstring >::const_iterator itr = args.m_Paths.begin(), end = args.m_Paths.end();
            itr != end; ++itr )
        {
            m_List->AddItem( *itr );
        }
        m_List->Read();
        m_List->Thaw();
    }
}
#endif