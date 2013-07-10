#include <stdio.h>

#include "apr.h"
#include "apr_strings.h"

#include "httpd.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_request.h"
#include "http_config.h"
#include "http_log.h"

#include "include/apreq.h"
#include "include/apreq_param.h"

#define PHPBBX_VIEW_FORUM "/viewforum.php"
#define PHPBBX_VIEW_TOPIC "/viewtopic.php"
#define PHPBBX_VIEW_USER "/profile.php"

#define TOPIC_PAGE_SIZE 250

static void register_hooks(apr_pool_t *pool);
static int ets_rewrite_handler(request_rec *r);

static const char* get_file(request_rec *r) {
        size_t fn_len = strlen(r->filename);
        char* scanner = r->filename + fn_len;
        while (*scanner != '/' && scanner >= r->filename) {
                scanner--;
        }
	return scanner;
}

static int redirect(request_rec* r, const char* remap) {
        apr_table_setn(r->headers_out, "Location", remap);
        return HTTP_MOVED_PERMANENTLY;
}

static const char*  build_id(request_rec *r, apr_table_t* get, char* key, int maxlen) {
        const char* id = apreq_params_as_string(r->pool, get, key, APREQ_JOIN_AS_IS);
	size_t len = id != NULL ? strlen(id) : 0;
	if (len == 0 || len > maxlen) {
		ap_log_rerror(APLOG_MARK, APLOG_CRIT, APR_EGENERAL, r, "ID missing or too long (%s = %lu)", key, (unsigned long) len);
		return NULL;
	} else {
	 	return id;
	}
}

static apr_table_t* get_get(request_rec *r) {
	apr_table_t* table = apr_table_make(r->pool, 0);
	
	if (APR_SUCCESS != apreq_parse_query_string(r->pool, table, r->args)) {
		ap_log_rerror(APLOG_MARK, APLOG_CRIT, APR_EGENERAL, r, "Could not build get...");
		return NULL;
	}
	
	return table;
}	 

static int map_topic(request_rec *r, apr_table_t* get) {
	const char* page = "index.html";
	const char* topic_id = build_id(r, get, "t", 6);
	const char* start = build_id(r, get, "start", 6);
	
	if (topic_id == NULL) {
		return (DECLINED);
	} 
	
	if (start != NULL) {
		int req_start = atoi(start);
		int page_idx = (req_start / TOPIC_PAGE_SIZE) * TOPIC_PAGE_SIZE;
		
		if (page_idx > 0) {
			page = apr_psprintf(r->pool, "index_page%d.html", page_idx);
		} 
		page = apr_psprintf(r->pool, "%s#%d", page, req_start % TOPIC_PAGE_SIZE);
	} 
		
	const char* remap = apr_psprintf(r->pool, "/topics/%s/%s", topic_id, page);
	ap_log_rerror(APLOG_MARK, APLOG_INFO, APR_EGENERAL, r, "Remap = %s", remap);	

	return redirect(r, remap);		
}

static int map_user(request_rec *r, apr_table_t *get) {
	apr_table_setn(r->headers_out, "Location", "http://altavista.com/");
	return HTTP_MOVED_PERMANENTLY;
}

static int map_forum(request_rec *r, apr_table_t *get) {
        const char* forum_id = build_id(r, get, "f", 6);

        if (forum_id == NULL) {
                return (DECLINED);
        }

        const char* remap = apr_psprintf(r->pool, "/forums/%s/index.html", forum_id);
        ap_log_rerror(APLOG_MARK, APLOG_INFO, APR_EGENERAL, r, "Remap = %s", remap);

        return redirect(r, remap);
}

static int ets_rewrite_handler(request_rec *r) {
	int (*map)(request_rec*,apr_table_t*) = NULL;

	if (!r->handler || strcmp(r->handler, "ets-rewrite-handler")) return (DECLINED);
	
	ap_log_rerror(APLOG_MARK, APLOG_CRIT, APR_EGENERAL, r, "Handler processing %s", r->filename);

	const char* req_file = get_file(r);

	ap_log_rerror(APLOG_MARK, APLOG_CRIT, APR_EGENERAL, r, "Scanner = %s", req_file);
			
	if (0 == strcmp(PHPBBX_VIEW_FORUM, req_file)) {
		map = &map_forum;
	} else if (0 == strcmp(PHPBBX_VIEW_TOPIC, req_file)) {
		map = &map_topic;
	} else if (0 == strcmp(PHPBBX_VIEW_USER, req_file)) {
		map = &map_user;
	} else {
		return (DECLINED);
	}

        apr_table_t *get = get_get(r);
        if (get == NULL) {
                return (DECLINED);
        }

	return (*map)(r, get);
}

static void register_hooks(apr_pool_t *pool) {
	ap_hook_handler(ets_rewrite_handler, NULL, NULL, APR_HOOK_LAST);
}

module AP_MODULE_DECLARE_DATA ets_rewrite_module =
{ 
    STANDARD20_MODULE_STUFF,
    NULL, // create_dir_conf, /* Per-directory configuration handler */
    NULL, //merge_dir_conf,  /* Merge handler for per-directory configurations */
    NULL, //create_svr_conf, /* Per-server configuration handler */
    NULL, //merge_svr_conf,  /* Merge handler for per-server configurations */
    NULL, //directives,      /* Any directives we may have for httpd */
    register_hooks   /* Our hook registering function */
};
