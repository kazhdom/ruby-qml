#include "util.h"
#include "conversion.h"
#include <QtCore/QString>

namespace RubyQml {

void protect(const std::function<void ()> &callback)
{
    auto f = [](VALUE data) {
        auto &callback = *reinterpret_cast<const std::function<void ()> *>(data);
        callback();
        return Qnil;
    };
    int state;
    rb_protect(f, reinterpret_cast<VALUE>(&callback), &state);
    if (state) {
        throw RubyException(state);
    }
}

void unprotect(const std::function<void ()> &callback)
{
    int state = 0;
    bool cxxEx = false;
    std::string cxxExMsg;
    try {
        callback();
    }
    catch (const RubyException &ex) {
        state = ex.state();
    }
    catch (const std::exception &ex) {
        cxxEx = true;
        cxxExMsg = ex.what();
    }
    if (state) {
        rb_jump_tag(state);
    }
    if (cxxEx) {
        rb_raise(rb_path2class("QML::CxxError"), "%s", cxxExMsg.c_str());
    }
}

void withoutGvl(const std::function<void ()> &callback)
{
    auto callbackp = const_cast<void *>(static_cast<const void *>(&callback));
    auto f = [](void *data) -> void * {
        auto &callback = *static_cast<const std::function<void ()> *>(data);
        callback();
        return nullptr;
    };
    rb_thread_call_without_gvl(f, callbackp, RUBY_UBF_IO, nullptr);
}

void withGvl(const std::function<void ()> &callback)
{
    auto callbackp = const_cast<void *>(static_cast<const void *>(&callback));
    auto f = [](void *data) -> void * {
        auto &callback = *static_cast<const std::function<void ()> *>(data);
        callback();
        return nullptr;
    };
    rb_thread_call_with_gvl(f, callbackp);
}

void fail(const char *errorClassName, const QString &message)
{
    auto msg = message.toUtf8();
    protect([&] {
        rb_raise(rb_path2class(errorClassName), "%s", msg.data());
    });
}

bool isKindOf(VALUE obj, VALUE klass)
{
    VALUE result;
    protect([&] {
        result = rb_obj_is_kind_of(obj, klass);
    });
    return fromRuby<bool>(result);
}

}
