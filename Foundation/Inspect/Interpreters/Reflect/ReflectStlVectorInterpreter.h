#pragma once

#include "ReflectFieldInterpreter.h"

#include "Foundation/Inspect/Canvas.h"
#include "Foundation/Inspect/Container.h"
#include "Foundation/Inspect/DataBinding.h"

namespace Helium
{
    namespace Inspect
    {
        // Forwards
        class Button;
        class List;
        typedef Helium::StrongPtr<Button> ButtonPtr;

        class HELIUM_FOUNDATION_API ReflectStlVectorInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectStlVectorInterpreter (Container* container);

            virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Object*>& instances, Container* parent) override;

        protected:
            virtual ButtonPtr AddAddButton( List* list );
            virtual ButtonPtr AddRemoveButton( List* list );
            virtual ButtonPtr AddMoveUpButton( List* list );
            virtual ButtonPtr AddMoveDownButton( List* list );

            // callbacks
            void OnAdd( const ButtonClickedArgs& args );
            void OnRemove( const ButtonClickedArgs& args );
            void OnMoveUp( const ButtonClickedArgs& args );
            void OnMoveDown( const ButtonClickedArgs& args );

            virtual void OnCreateFieldData( Reflect::DataPtr & s ){ }
        };

        typedef Helium::SmartPtr<ReflectStlVectorInterpreter> ReflectStlVectorInterpreterPtr;
        typedef std::vector< ReflectStlVectorInterpreterPtr > V_ReflectStlVectorInterpreterSmartPtr;
    }
}