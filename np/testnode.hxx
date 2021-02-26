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
#ifndef __NP_TESTNODE_H__
#define __NP_TESTNODE_H__ 1

#include "np/util/common.hxx"
#include "np/types.hxx"
#include "np/spiegel/spiegel.hxx"
#include <list>

namespace np
{

    class testnode_t : public np::util::zalloc
    {
      public:
        testnode_t(const char *);
        ~testnode_t();

        std::string get_fullname() const;
        testnode_t *get_parent()
        {
            return parent_;
        }
        testnode_t *find(const char *name);
        testnode_t *make_path(std::string name);
        void set_function(functype_t, np::spiegel::function_t *);
        void add_mock(np::spiegel::function_t *target, np::spiegel::function_t *mock);
        void add_mock(np::spiegel::addr_t target, const char *name, np::spiegel::addr_t mock);
        void add_mock(np::spiegel::addr_t target, np::spiegel::addr_t mock);

        testnode_t *detach_common();
        np::spiegel::function_t *get_function(functype_t type) const
        {
            return funcs_[type];
        }
        std::list<np::spiegel::function_t *> get_fixtures(functype_t type) const;
        void pre_run() const;
        void post_run() const;

        void dump(int level) const;

        struct parameter_t
        {
            parameter_t(const char *, char **, const char *);
            ~parameter_t();

            // I tried using std::string for name_ and values_ but the
            // string memory management is sufficiently obscure that it
            // results in valgrind in the forked child reporting leaks.

            char *name_;
            char **variable_;
            std::vector<char *> values_;
        };

        class assignment_t
        {
          public:
            assignment_t(const parameter_t *p) : param_(p), idx_(0) {}
            void apply() const;
            void unapply() const;
            std::string as_string() const;

          private:
            const parameter_t *param_;
            unsigned int idx_;

            friend bool bump(std::vector<testnode_t::assignment_t>& a);
            friend int operator==(const std::vector<testnode_t::assignment_t>& a,
                                  const std::vector<testnode_t::assignment_t>& b);
        };

        void add_parameter(const char *, char **, const char *);
        std::vector<assignment_t> create_assignments() const;

        class preorder_iterator
        {
          public:
            preorder_iterator()
                : base_(0), node_(0) {}
            preorder_iterator(testnode_t *n)
                : base_(n), node_(n) {}
            testnode_t *operator* () const
            {
                return node_;
            }
            preorder_iterator& operator++();
            int operator== (const preorder_iterator& o) const
            {
                return o.node_ == node_;
            }
            int operator!= (const preorder_iterator& o) const
            {
                return o.node_ != node_;
            }
            preorder_iterator& operator=(testnode_t *n)
            {
                base_ = node_ = n;
                return *this;
            }
          private:
            testnode_t *base_;
            testnode_t *node_;
        };
        preorder_iterator preorder_begin()
        {
            return preorder_iterator(this);
        }
        preorder_iterator preorder_end()
        {
            return preorder_iterator();
        }

      private:
        bool is_elidable() const;

        testnode_t *next_;
        testnode_t *parent_;
        testnode_t *children_;
        char *name_;
        np::spiegel::function_t *funcs_[FT_NUM_SINGULAR];
        std::vector<np::spiegel::intercept_t *> intercepts_;
        std::vector<parameter_t *> parameters_;

        friend class preorder_iterator;
    };

    bool bump(std::vector<testnode_t::assignment_t>& a);
    int operator==(const std::vector<testnode_t::assignment_t>& a,
                   const std::vector<testnode_t::assignment_t>& b);

    // close the namespace
};

#endif /* __NP_TESTNODE_H__ */
