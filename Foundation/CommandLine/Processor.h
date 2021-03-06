#pragma once

#include "Foundation/API.h"
#include "Foundation/CommandLine/Option.h"
#include "Foundation/CommandLine/Command.h"

/******************************************************************************

Option
- Token
- Usage
- Help
- Value (<class T>)

Command
- Token
- Usage
- ShortHelp
- [Dynamic]Help
- Options
* Func: Un/Register Options
* Func: Parse - Parse command line
* Func: Process - Do the command with parsed options
  
Processor
- Token
- Usage
- ShortHelp
- [Dynamic]Help
- Options
- Commands
* Func: Un/Register Options
* Func: Un/Register Commands
* Func: Parse - Parse command line
* Func: Process - Do the command with parsed options

******************************************************************************/

namespace Helium
{
    namespace CommandLine
    {
		class HELIUM_FOUNDATION_API Processor
        {
        protected:
			tstring m_Token;
			tstring m_Usage;
			tstring m_ShortHelp;
			mutable tstring m_Help;

			OptionsMap m_OptionsMap;
            M_StringToCommandDumbPtr m_Commands;

        public:
            Processor( const tchar_t* token, const tchar_t* usage = TXT( "COMMAND [ARGS]" ), const tchar_t* shortHelp = TXT( "" ) );
            virtual ~Processor();

			virtual bool Initialize( tstring& error )
            {
                return true;
            }

            const tstring& Token() const
			{
				return m_Token;
			}

			const tstring& ShortHelp() const
			{
				return m_ShortHelp;
			}

            virtual const tstring& Help() const;

            bool AddOption( const OptionPtr& option, tstring& error );
            bool ParseOptions( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error );

            bool RegisterCommand( Command* command, tstring& error );
            Command* GetCommand( const tstring& token );

			virtual bool Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error );
        };
    }
}
