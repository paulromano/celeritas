//----------------------------------*-C++-*----------------------------------//
// Copyright 2020 UT-Battelle, LLC, and other Celeritas developers.
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: (Apache-2.0 OR MIT)
//---------------------------------------------------------------------------//
//! \file TypeDemangler.test.cc
//---------------------------------------------------------------------------//
#include "base/TypeDemangler.hh"

#include "celeritas_test.hh"

using celeritas::demangled_typeid_name;
using celeritas::TypeDemangler;

//---------------------------------------------------------------------------//

namespace tdtest
{
template<class T>
struct FlorbyDorb
{
};

struct Zanzibar
{
};

struct JapaneseIsland
{
    virtual ~JapaneseIsland() {}
};

struct Honshu : public JapaneseIsland
{
};

struct Hokkaido : public JapaneseIsland
{
};
} // namespace tdtest

//---------------------------------------------------------------------------//
// TESTS
//---------------------------------------------------------------------------//

TEST(TypeDemanglerTest, demangled_typeid_name)
{
    std::string int_type = demangled_typeid_name(typeid(int).name());
    std::string flt_type = demangled_typeid_name(typeid(float).name());

    EXPECT_NE(int_type, flt_type);

#ifdef __GNUG__
    EXPECT_EQ("int", int_type);
    EXPECT_EQ("float", flt_type);
#endif
}

TEST(TypeDemanglerTest, static_types)
{
    using namespace tdtest;

    TypeDemangler<FlorbyDorb<Zanzibar>> demangle_type;

    std::string fdz_type = demangle_type();
    EXPECT_NE(fdz_type, TypeDemangler<FlorbyDorb<Hokkaido>>()());

#ifdef __GNUG__
    EXPECT_EQ("tdtest::FlorbyDorb<tdtest::Zanzibar>", fdz_type);
#endif
}

TEST(TypeDemanglerTest, dynamic)
{
    using namespace tdtest;

    TypeDemangler<JapaneseIsland> demangle;
    const Honshu                  honshu;
    const Hokkaido                hokkaido;
    const JapaneseIsland&         hon_ptr = honshu;
    const JapaneseIsland&         hok_ptr = hokkaido;

    EXPECT_EQ(demangle(honshu), demangle(hon_ptr));
    EXPECT_EQ(demangle(hokkaido), demangle(hok_ptr));
    EXPECT_NE(demangle(JapaneseIsland()), demangle(hon_ptr));

#ifdef __GNUG__
    EXPECT_EQ("tdtest::Hokkaido", demangle(hok_ptr));
#endif
}
