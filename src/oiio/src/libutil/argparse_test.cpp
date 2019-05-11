/*
  Copyright 2019 Larry Gritz and the other authors and contributors.
  All Rights Reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are
  met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of the software's owners nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  (This is the Modified BSD License)
*/

#include <vector>

#include <OpenImageIO/argparse.h>
#include <OpenImageIO/strutil.h>
#include <OpenImageIO/unittest.h>
#include <OpenImageIO/ustring.h>

using namespace OIIO;



// Split a command line into a vector of const char* arguments.
std::vector<const char*>
split_commands(string_view commands)
{
    std::vector<const char*> result;
    for (auto& c : Strutil::splitsv(commands)) {
        result.push_back(ustring(c).c_str());
    }
    return result;
}



static std::vector<std::string> prearg;
static std::vector<std::string> postarg;
static std::vector<std::string> callbacklist;


static int
parse_prearg(int argc, const char* argv[])
{
    for (int i = 0; i < argc; i++)
        prearg.emplace_back(argv[i]);
    return 0;
}


static int
parse_postarg(int argc, const char* argv[])
{
    for (int i = 0; i < argc; i++)
        postarg.emplace_back(argv[i]);
    return 0;
}



static int
callback(int argc, const char* argv[])
{
    for (int i = 0; i < argc; i++) {
        // std::cout << "callback " << argv[i] << "\n";
        callbacklist.emplace_back(argv[i]);
    }
    return 0;
}



static void
test_basic()
{
    auto args   = split_commands("basic alpha --flag --unflag --intarg 42 "
                               "--floatarg 3.5 --stringarg foo "
                               "--append xxx --append yyy "
                               "--callback who "
                               "bravo charlie");
    bool flag   = false;
    bool unflag = true;
    int i       = 0;
    float f     = 0;
    std::string s;
    std::vector<std::string> list;
    prearg.clear();
    postarg.clear();
    callbacklist.clear();

    ArgParse ap;
    // clang-format off
    ap.options(
        "basic",
        "%1", parse_prearg, "",
        "%*", parse_postarg, "",
        "--flag", &flag, "Set flag",
        "--unflag %!", &unflag, "Unset flag",
        "--intarg %d", &i, "int",
        "--floatarg %f", &f, "float",
        "--stringarg %s", &s, "string",
        "--callback %@ %s", callback, nullptr, "callback",
        "--append %L", &list, "string list",
        nullptr);
    ap.usage();
    // clang-format on

    ap.parse(int(args.size()), args.data());
    OIIO_CHECK_EQUAL(flag, true);
    OIIO_CHECK_EQUAL(unflag, false);
    OIIO_CHECK_EQUAL(i, 42);
    OIIO_CHECK_EQUAL(f, 3.5f);
    OIIO_CHECK_EQUAL(s, "foo");
    OIIO_CHECK_EQUAL(list.size(), 2);
    if (list.size() == 2) {
        OIIO_CHECK_EQUAL(list[0], "xxx");
        OIIO_CHECK_EQUAL(list[1], "yyy");
    }
    OIIO_CHECK_EQUAL(prearg.size(), 1);
    if (prearg.size() >= 1) {
        OIIO_CHECK_EQUAL(prearg[0], "alpha");
    }
    OIIO_CHECK_EQUAL(postarg.size(), 2);
    if (postarg.size() >= 2) {
        OIIO_CHECK_EQUAL(postarg[0], "bravo");
        OIIO_CHECK_EQUAL(postarg[1], "charlie");
    }
    OIIO_CHECK_EQUAL(callbacklist.size(), 2);
    if (callbacklist.size() >= 2) {
        OIIO_CHECK_EQUAL(callbacklist[0], "--callback");
        OIIO_CHECK_EQUAL(callbacklist[1], "who");
    }
}



int
main(int argc, char* argv[])
{
    test_basic();

    return unit_test_failures != 0;
}
