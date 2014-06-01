#pragma once

#include "extbase.h"
#include <memory>

class QPluginLoader;

namespace RubyQml {
namespace Ext {

class PluginLoader : public ExtBase<PluginLoader>
{
    friend class ExtBase<PluginLoader>;

public:
    PluginLoader();
    ~PluginLoader();

    VALUE initialize(VALUE path);
    VALUE load();
    VALUE instance();

private:
    void mark() {}
    static ClassBuilder buildClass();
    std::unique_ptr<QPluginLoader> mPluginLoader;
};

} // namespace Ext
} // namespace RubyQml
