/*
 * Copyright 2011-2012 Gregory Banks
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "abbrev.hxx"
#include "reader.hxx"

namespace np
{
    namespace spiegel
    {
        namespace dwarf
        {
            using namespace std;

            bool abbrev_t::read(reader_t &r)
            {
                if(!r.read_uleb128(tag) ||
                        !r.read_u8(children))
                {
                    return false;
                }
                for(;;)
                {
                    attr_spec_t as;
                    if(!r.read_uleb128(as.name) ||
                            !r.read_uleb128(as.form))
                    {
                        return false;
                    }
                    if(!as.name && !as.form)
                    {
                        break;
                    }      /* name=0, form=0 indicates end
             * of attribute specifications */
                    attr_specs.push_back(as);
                }
                return true;
            }

            // close namespaces
        };
    };
};
