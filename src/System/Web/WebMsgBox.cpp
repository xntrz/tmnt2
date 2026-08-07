#include "WebMsgBox.hpp"
#include "WebTypedefs.hpp"

#include <emscripten.h>


static CWebMsgBox::ANSWER s_webMsgBoxAnswer;


EMSCRIPTEN_KEEPALIVE
extern "C" void webMsgBoxCallback(int32 result)
{
    s_webMsgBoxAnswer = static_cast<CWebMsgBox::ANSWER>(result);
};


#define webMsgBoxCallbackName TOSTRING(webMsgBoxCallback)


/*static*/ CWebMsgBox::ANSWER
CWebMsgBox::Invoke(TYPE type, const char* pszTitle, const char* pszText)
{
    s_webMsgBoxAnswer = CWebMsgBox::ANSWER_OK;

    switch (type)
    {
    case TYPE_OK:
        {
            EM_ASM({
                var callback = UTF8ToString($1);
                
                alert([UTF8ToString($0)]);
                ccall(callback, 'v', ['number'], 0);
            }, pszText, webMsgBoxCallbackName);
        }
        break;

    case TYPE_OK_CANCEL:
        {
            EM_ASM({
                var callback = UTF8ToString($1);
            
                if (confirm([UTF8ToString($0)])) {
                    ccall(callback, 'v', ['number'], 0);
                }
                else {
                    ccall(callback, 'v', ['number'], 1);
                };
            }, pszText, webMsgBoxCallbackName);
        }
        break;

    default:
        break;
    };

    return s_webMsgBoxAnswer;
};