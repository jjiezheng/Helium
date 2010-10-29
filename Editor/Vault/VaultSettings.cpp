#include "Precompile.h"
#include "VaultSettings.h"
#include "Editor/Vault/VaultPanel.h"
#include "Foundation/Math/Utils.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
VaultSettings::VaultSettings( VaultViewMode viewVaultMode, uint32_t thumbnailSize )
: m_VaultViewMode( viewVaultMode )
, m_ThumbnailSize( thumbnailSize )
//, m_WindowSettings( new WindowSettings() )
{
}

VaultSettings::~VaultSettings()
{
}
