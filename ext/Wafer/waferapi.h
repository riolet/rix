#ifndef WAFERAPI_H_
#define WAFERAPI_H_
/* ENTL */
#define STR(X) #X
#define WSPC " "
#define CRLF "\r\n"
#define ATTR(key,value) STR(key) STR(=) STR(value)

#define LT(tag) STR(<) STR(tag) WSPC    /*<tag */
#define LTA(tag,attributes) LT(tag) STR(attributes) WSPC        /*<tag */
#define GT STR(>)               /*< */

#define OTAG(tag) LT(tag) GT    /*<tag> */
#define OTAGA(tag,attributes) LTA(tag,attributes) GT    /*<tag attributes> */
#define CTAG(tag) STR(</) STR(tag) GT   /*</tag> */

#define ESTAG(tag) LT(tag) WSPC STR(/) GT       /*<tag attributes /> */
#define STAG(tag,attributes) LTA(tag,attributes) WSPC STR(/) GT /*<tag attributes /> */

#define QTAG(tag,text) OTAG(tag) text CTAG(tag) /*<tag> text </tag> */
#define QTAGA(tag,attributes,text) OTAGA(tag,attributes) text CTAG(tag) /*<tag attributes> text </tag> */

/* Extensions */
#define QLINK(url,text) QTAGA(a,href=url,text)
#define QLINKA(url,attributes,text) QTAGA(a,href=url attributes,text)
#define QIMG(srcurl) STAG(img,src=srcurl)
#define QIMGA(srcurl) STAG(img,src=srcurl attributes)
#define QBR ESTAG(br)

#define STAGPARAMQ(tag,attributes) LTA(tag,attributes) ATTR(name,STR(q)) WSPC STR(/) GT /*<tag attributes name="q" /> */
#define QTAGAPARAMQ(tag,attributes,text) OTAGA(tag,attributes name=STR(q)) text CTAG(tag)       /*<tag attributes> text </tag> */
/* End ENTL */

long resPrintf(Response *response, const char *format, ...);
void serveFile(Response *response, const char *filename, const char *displayFilename,
                           const char *type);

void sendStatusOKHeadersTypeEncoding(Response *response, const char *type, const char *encoding);
void sendResourceNotFound(Response *response);

char *resQuickForm(Request *request, Response *response, const char *msg, const char *inputstr);
#define QUICK_FORM_TEXT(request,response,msg) resQuickForm(request,response,msg,STAGPARAMQ(input,type="text"))

char *getQueryParam(Request *request, const char *name);
char *getQueryPath(Request *request);
bool routeRequest(Request *request, Response * response, const char *path, void (*function) (Request *, Response *));
long resPuts(Response * response, const char *buffer);
/*Internal stuff follows. Could change in future. Do not use */
#define STATIC_SEND(_socket, _str) send(_socket, _str, sizeof(_str)-1, 0)
#define API_FLAGS_HEADER_SENT 1
#define API_FLAGS_DONT_SET_HEADER_BEFORE_SENDING API_FLAGS_HEADER_SENT*2
#define API_FLAGS_FORM_ONLY_ON_NULL API_FLAGS_DONT_SET_HEADER_BEFORE_SENDING*2
#endif
