#include "Structure.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium::Reflect;

Structure::Structure ()
{

}

Structure::~Structure ()
{
    delete m_Template;
}

Structure* Structure::Create()
{
    return new Structure();
}
