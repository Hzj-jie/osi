#pragma once

namespace osi
{
    namespace compiler
    {
        class features_t
        {
        public:
            virtual ~features_t() = default;

            virtual bool with_type_alias() const
            {
                return true;
            }

            virtual bool with_namespace() const
            {
                return true;
            }
        };
    }
}
