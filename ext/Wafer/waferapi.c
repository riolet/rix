#include "wafer.h"
#include "waferapi.h"

/* Ver 0.0.5 Functions
 * Parameters: request, format, (optional) variadargs
 * Returns: the number of characters printed
 */

/*Internal functions don't use */
ssize_t writeLongString(int client, const char *longString, size_t len);
static void sendFileWithSelect(int client, int read_fd, struct stat stat_buf);
/*End internal functions */

long resPrintf(Response * response, const char *format, ...)
{
	dbgprintf("Flags %d API Flags% d response.status %d\n",response->flags,response->apiFlags,response->status);
	if (!(response->apiFlags&API_FLAGS_HEADER_SENT)) {
		if (!(response->apiFlags&API_FLAGS_DONT_SET_HEADER_BEFORE_SENDING)) {
			response->apiFlags|=API_FLAGS_HEADER_SENT;
			sendStatusOKHeadersTypeEncoding(response,"text/html",NULL);
		} else {

			/*sendHeadersResponse*/
		}
	}
    /* initial buffer large enough for most cases, will resize if required */
    char *buf = malloc(MAX_BUFFER_SIZE);
    int len;

    va_list arg;
    long done;
    va_start(arg, format);
    len = vsnprintf(buf, MAX_BUFFER_SIZE, format, arg);
    va_end(arg);

    if (len > MAX_BUFFER_SIZE) {
        /* buffer size was not enough */
        free(buf);
        buf = malloc(len + 1);
        if (buf == NULL) {
            printf("Could not allocate memory.");
            exit(EXIT_FAILURE);
        }
        va_start(arg, format);
        vsnprintf(buf, len + 1, format, arg);
        va_end(arg);
    }

    if (len < MAX_BUFFER_SIZE) {
        done = (int)send(response->fd, buf, len, 0);
    } else {
        done = writeLongString(response->fd, buf, len);
    }

    free(buf);
    return done;
}

/*Writes the given null-terminated string to the compressed file, excluding the terminating null character.
Returns the number of characters written, or -1 in case of error */
long resPuts(Response * response, const char *buffer)
{
	if (!(response->apiFlags&API_FLAGS_HEADER_SENT)) {
		if (!(response->apiFlags&API_FLAGS_DONT_SET_HEADER_BEFORE_SENDING)) {
			response->apiFlags|=API_FLAGS_HEADER_SENT;
			sendStatusOKHeadersTypeEncoding(response,"text/html",NULL);
		} else {
			/*sendHeadersResponse*/
		}
	}
	return send(response->fd, buffer, strlen(buffer), 0);
}

/* Serve file. */
/* If displayFilename is not null, the file will be downloadable */
void serveFile(Response * response, const char *filename, const char *displayFilename,
                           const char *type)
{
    FILE *pFile = NULL;
    response->apiFlags |= API_FLAGS_DONT_SET_HEADER_BEFORE_SENDING;

    pFile = fopen(filename, "r");
    if (pFile == NULL) {
    	sendResourceNotFound(response);
    } else {
        int read_fd = fileno(pFile);
        struct stat stat_buf;
        fstat (read_fd, &stat_buf);

        STATIC_SEND(response->fd, "HTTP/1.0 200 OK\r\n");
        STATIC_SEND(response->fd, SERVER_STRING);

        resPrintf(response, "Content-Type: %s\r\n", type);
        resPrintf(response, "Content-Length: %d\r\n", stat_buf.st_size);
        if (displayFilename!=NULL) {
            resPrintf(response, "Content-Disposition: attachment; filename=\"%s\"\r\n",
                 displayFilename);
        }

        STATIC_SEND(response->fd, "\r\n");

        sendFileWithSelect(response->fd, read_fd, stat_buf);
    }
    fclose(pFile);
}

void sendStatusOKHeadersTypeEncoding(Response * response, const char *type, const char *encoding)
{
    STATIC_SEND(response->fd, "HTTP/1.0 200 OK\r\n");
    STATIC_SEND(response->fd, SERVER_STRING);
    resPrintf(response, "Content-Type: %s\r\n", type);
    if (encoding != NULL) {
        resPrintf(response, "Content-Encoding: %s\r\n", encoding);
    }
    STATIC_SEND(response->fd, "Vary: Accept-Encoding\r\n");
    STATIC_SEND(response->fd, "\r\n");
    response->status=STATUS_HTTP_OK;
}

void sendResourceNotFound(Response * response)
{
	STATIC_SEND(response->fd, "HTTP/1.0 404 NOT FOUND\r\n");
	STATIC_SEND(response->fd, "Content-Type: text/html\r\n");
	STATIC_SEND(response->fd, "\r\n");
	STATIC_SEND(response->fd, "<HTML><TITLE>Not Found</TITLE>\r\n");
	STATIC_SEND(response->fd, "<BODY><P>The server could not fulfill\r\n");
	STATIC_SEND(response->fd, "your request because the resource specified\r\n");
	STATIC_SEND(response->fd, "is unavailable or nonexistent.\r\n");
	STATIC_SEND(response->fd, "</P></BODY></HTML>\r\n");
	response->status=STATUS_HTTP_NOT_FOUND;
}

/*Input Functions */
char *resQuickForm(Request *request, Response *response, const char *msg, const char *inputstr)
{
    char *qpath = getQueryPath(request);
    char *qparam = getQueryParam(request,"q");
    if (!((API_FLAGS_FORM_ONLY_ON_NULL&response->apiFlags)&&qparam != NULL)) {
        resPrintf(response, OTAGA(form, action = "%s")
                 "%s%s" STAG(input, type = "submit") CTAG(form), qpath, msg, inputstr);
    }
    free(qpath);
    return qparam;
}

/* Utility Functions */
char *getHeader(char **headers, char *header)
{
    char *current_header, *matching_header, *value;
    // Not sure if MAX_BUFFER_SIZE is right.
    char *retval = malloc(MAX_BUFFER_SIZE * sizeof(char));
    if (retval == NULL) {
        printf("Could not allocate memory.");
        exit(EXIT_FAILURE);
    }
    int i;
    for (i = 0; headers[i] != NULL; i++) {
        current_header = headers[i];
        if ((matching_header = strstr(current_header, header))) {
            value = matching_header + strlen(header);
            if (*value == ':') {
                while (*value == ' ' || *value == ':') {
                    value++;
                }
                strncpy(retval, value, MAX_BUFFER_SIZE);
                return retval;
            }
        }
    }
    memcpy(retval, UNDEFINED, sizeof(UNDEFINED));
    return retval;
}

/**********************************************************************/
/* Return the value of a query parameter.
 * Parameters: the query string
 *             the name of the parameter
 * Returns: the value of the query parameter */
/**********************************************************************/

char *getQueryParam(Request *request, const char *name)
{
    char bufferAmpersand[MAX_BUFFER_SIZE];
    char bufferQuestion[MAX_BUFFER_SIZE];
    snprintf(bufferQuestion, MAX_BUFFER_SIZE - 1, "?%s=", name);
    snprintf(bufferAmpersand, MAX_BUFFER_SIZE - 1, "&%s=", name);
    char *buffer;
    char *pos1;

    char *value = malloc(MAX_BUFFER_SIZE * sizeof(char));
    if (value == NULL) {
        printf("Could not allocate memory.");
        exit(EXIT_FAILURE);
    }
    int i;

    buffer = bufferQuestion;
    pos1 = strstr(request->reqStr, bufferQuestion);
    dbgprintf("Buffer %s Pos %s\n", buffer, pos1);
    if (!pos1) {
        buffer = bufferAmpersand;
        pos1 = strstr(request->reqStr, bufferAmpersand);
    }
    if (pos1) {
        pos1 += strlen(buffer);
        i = 0;
        while (*pos1 && *pos1 != '&' && i < MAX_BUFFER_SIZE) {
            if (*pos1 == '%') {
                value[i] = (char)ToHex(pos1[1]) * 16 + ToHex(pos1[2]);
                pos1 += 3;
            } else if (*pos1 == '+') {
                value[i] = ' ';
                pos1++;
            } else {
                value[i] = *pos1++;
            }
            i++;
        }
        value[i] = '\0';
        return value;
    }

    free(value);
    value=NULL;
    return value;
}

/**********************************************************************/
/* Return the query path. For example the query path for
 * http://waferdotc.com/faq is /faq
 * Note the / at the beginning
 * Parameters: the request
 * Returns: the query path */
/**********************************************************************/

char *getQueryPath(Request *request)
{
    char *queryPath;
    queryPath = strdup(request->reqStr);
    u_int i;
    for (i = 0; i < strlen(request->reqStr) && (queryPath[i] != '?') && (queryPath[i] != '\0');
         i++) {
    }

    queryPath[i] = '\0';

    return queryPath;
}

bool routeRequest(Request *request, Response * response, const char *path, void (*function) (Request *, Response *))
{
    char *queryPath = getQueryPath(request);
    if (strcmp(queryPath, path) == 0) {
        free(queryPath);
        if (function != NULL)
            function(request, response);
        return true;
    } else {
        free(queryPath);
        return false;
    }
}

/*Internal stuff follows. Could change in future. Do not use */
 #ifndef __linux__
ssize_t sendfile_wafer (int write_fd, int read_fd, off_t *offset,int remain)
{
    char buf[MAX_BUFFER_SIZE];
    lseek(read_fd, *offset, SEEK_SET);
    ssize_t bytes_read = read(read_fd, buf, MAX_BUFFER_SIZE);
    ssize_t bytes_written = write(write_fd, buf, bytes_read);
    return bytes_written;
}
#endif // __linux__

static void sendFileWithSelect(int write_fd, int read_fd, struct stat stat_buf)
{
    off_t offset = 0;

    size_t remain = stat_buf.st_size;
    size_t sent = 0;
    fd_set write_fds;
    FD_ZERO(&write_fds);
    FD_SET(write_fd,&write_fds);
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

     while (remain) {
        dbgprintf("Remain %d out of %d \n",remain,stat_buf.st_size);
        int selected = select (write_fd + 1, NULL, &write_fds, NULL, &timeout);
        if (selected < 0) {
            return; //TODO-Error Logging
        } else if (selected == 0) {
            dbgprintf("Timed out. Going back in \n");
            continue; //Timed out. Loop again.
        }
        #ifdef __linux__
        ssize_t sent_once = sendfile (write_fd, read_fd, &offset, remain);
        #else
        ssize_t sent_once = sendfile_wafer (write_fd, read_fd, &offset, remain);
        #endif // __linux__
        if (sent_once <= 0)
            return;
        sent += sent_once;
        offset = sent;
        remain -= sent_once;
     }


     /* Close up. */
     close (read_fd);
}

ssize_t writeLongString(int client, const char *longString, size_t len)
{
    size_t remain = len;
    size_t sent = 0;

    while (remain) {
        size_t toCpy = remain > MAX_BUFFER_SIZE ? MAX_BUFFER_SIZE : remain;
        ssize_t sent_once = send(client, longString, toCpy, 0);
        if (sent_once <= 0)
            return -1;
        sent += sent_once;
        longString += sent_once;
        remain -= sent_once;
    }
    return sent;
}
