// Copyright (c) 2016-2018, Pacific Biosciences of California, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted (subject to the limitations in the
// disclaimer below) provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//  * Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//  * Neither the name of Pacific Biosciences nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE
// GRANTED BY THIS LICENSE. THIS SOFTWARE IS PROVIDED BY PACIFIC
// BIOSCIENCES AND ITS CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL PACIFIC BIOSCIENCES OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
// USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
// OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.

// Author: Derek Barnett

#ifndef PBCOPPER_CLI_OPTION_INL_H
#define PBCOPPER_CLI_OPTION_INL_H

#include <pbcopper/cli/Option.h>

namespace PacBio {
namespace CLI {
namespace internal {

class OptionPrivate
{
public:
    PacBio::JSON::Json option_;

public:
    explicit OptionPrivate(const std::string& id,
                           const std::vector<std::string>& names,
                           const std::string& description,
                           const PacBio::JSON::Json& defaultValue,
                           const PacBio::JSON::Json& choices,
                           const OptionFlags& flags)
        : option_(PacBio::JSON::Json::object_t())
    {
        // validate ID & name(s)
        if (id.empty())
            throw std::runtime_error("CLI::Option: options must have at non-empty ID");
        if (names.empty())
            throw std::runtime_error("CLI::Option: options must have at least one name");
        for (const std::string& name : names) {
            if (name.empty())
                throw std::runtime_error("CLI::Option: options cannot be empty");
            if (name.at(0) == '-')
                throw std::runtime_error("CLI::Option: options cannot start with '-'");
            if (name.at(0) == '/')
                throw std::runtime_error("CLI::Option: options cannot start with '/'");
            if (name.find('=') != std::string::npos)
                throw std::runtime_error("CLI::Option: options cannot contain an '='");
        }

        // store data
        option_["id"]           = id;
        option_["names"]        = names;
        option_["description"]  = description;

        // if none provided, treat as a switch-type option (init w/ false)
        if (defaultValue.is_null())
            option_["defaultValue"] = PacBio::JSON::Json::boolean_t(false);
        else
            option_["defaultValue"] = defaultValue;

        // choices
        if (choices.is_array() && !choices.empty())
            option_["choices"] = choices;
        else
            option_["choices"] = JSON::Json(nullptr);

        // flags
        const bool isHidden = (flags & OptionFlags::HIDE_FROM_HELP) != 0;
        option_["hidden"] = isHidden;
    }

    OptionPrivate(const OptionPrivate& other)
        : option_(other.option_)
    { }
};

} // namespace internal

// ------------------------
// PacBio::CLI::Option
// ------------------------

inline Option::Option(const std::string& id,
                      const std::string& name,
                      const std::string& description,
                      const PacBio::JSON::Json& defaultValue,
                      const JSON::Json& choices,
                      const OptionFlags& flags)
    : d_(new internal::OptionPrivate{
            id,
            std::vector<std::string>{1, name},
            description,
            defaultValue,
            choices,
            flags
         })
{ }

inline Option::Option(const std::string& id,
                      const std::initializer_list<std::string>& init,
                      const std::string& description,
                      const PacBio::JSON::Json& defaultValue,
                      const PacBio::JSON::Json& choices,
                      const OptionFlags& flags)
    : Option(id,
             std::vector<std::string>{init},
             description,
             defaultValue,
             choices,
             flags)
{ }

inline Option::Option(const std::string& id,
                      const std::vector<std::string>& names,
                      const std::string& description,
                      const PacBio::JSON::Json& defaultValue,
                      const PacBio::JSON::Json& choices,
                      const OptionFlags& flags)
    : d_(new internal::OptionPrivate{
            id,
            names,
            description,
            defaultValue,
            choices,
            flags
         })
{ }

inline Option::Option(const Option& other)
    : d_(new internal::OptionPrivate(*other.d_.get()))
{ }

inline Option::Option(Option&& other)
    : d_(std::move(other.d_))
{ }

inline Option& Option::operator=(const Option& other)
{
    d_->option_ = other.d_->option_;
    return *this;
}

inline Option& Option::operator=(Option&& other)
{
    std::swap(d_, other.d_);
    return *this;
}

inline Option::~Option(void) { }

inline PacBio::JSON::Json Option::Choices(void) const
{ return d_->option_["choices"]; }

inline PacBio::JSON::Json Option::DefaultValue(void) const
{ return d_->option_["defaultValue"]; }

inline std::string Option::Description(void) const
{ return d_->option_["description"]; }

inline bool Option::HasChoices(void) const
{
    const auto& choices = Choices();
    return choices.is_array() && !choices.empty();
}

inline std::string Option::Id(void) const
{ return d_->option_["id"]; }

inline bool Option::IsHidden(void) const
{ return d_->option_["hidden"]; }

inline std::vector<std::string> Option::Names(void) const
{ return d_->option_["names"]; }

inline std::string Option::ValueName(void) const
{ return std::string(); }

} // namespace CLI
} // namespace PacBio

#endif // PBCOPPER_CLI_OPTION_INL_H
