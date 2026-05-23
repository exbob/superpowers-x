#include "config.h"

#include <tomlc17.h>

#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_MAX_FILE_SIZE (1024U * 1024U)

static char *dup_string(const char *src)
{
	size_t len;
	char *dst;

	if (!src) {
		return NULL;
	}
	len = strlen(src);
	dst = (char *)malloc(len + 1);
	if (!dst) {
		return NULL;
	}
	memcpy(dst, src, len + 1);
	return dst;
}

static char *read_all(FILE *fp, int *too_large)
{
	size_t used = 0;
	size_t cap = 1024;
	char *buf = (char *)malloc(cap);
	size_t nread;

	if (too_large) {
		*too_large = 0;
	}
	if (!buf) {
		return NULL;
	}

	for (;;) {
		if (used > SIZE_MAX - 512 - 1) {
			free(buf);
			return NULL;
		}
		if (used + 512 + 1 > cap) {
			size_t next_cap = cap * 2;
			if (cap > SIZE_MAX / 2) {
				next_cap = used + 512 + 1;
			}
			if (next_cap < used + 512 + 1) {
				free(buf);
				return NULL;
			}
			char *next = (char *)realloc(buf, next_cap);
			if (!next) {
				free(buf);
				return NULL;
			}
			buf = next;
			cap = next_cap;
		}
		nread = fread(buf + used, 1, 512, fp);
		used += nread;
		if (used > CONFIG_MAX_FILE_SIZE) {
			free(buf);
			if (too_large) {
				*too_large = 1;
			}
			return NULL;
		}
		if (nread < 512) {
			if (ferror(fp)) {
				free(buf);
				return NULL;
			}
			break;
		}
	}
	buf[used] = '\0';
	return buf;
}

static int is_known_root_key(const char *key)
{
	return strcmp(key, "interval_seconds") == 0 ||
	       strcmp(key, "zlog_config_path") == 0;
}

static int has_only_known_members(toml_datum_t root)
{
	int i;

	if (root.type != TOML_TABLE) {
		return 0;
	}
	for (i = 0; i < root.u.tab.size; ++i) {
		if (!is_known_root_key(root.u.tab.key[i])) {
			return 0;
		}
	}
	return 1;
}

static int parse_interval_seconds(toml_datum_t root, unsigned int *out_interval)
{
	toml_datum_t interval_item;
	int64_t interval_value;

	interval_item = toml_get(root, "interval_seconds");
	if (interval_item.type != TOML_INT64) {
		return CONFIG_ERR_SCHEMA;
	}

	interval_value = interval_item.u.int64;
	if (interval_value < 1 || interval_value > (int64_t)UINT_MAX) {
		return CONFIG_ERR_SCHEMA;
	}

	*out_interval = (unsigned int)interval_value;
	return CONFIG_OK;
}

static int parse_zlog_config_path(toml_datum_t root, char **out_path_copy)
{
	toml_datum_t path_item;
	const char *path_text;
	char *path_copy;

	path_item = toml_get(root, "zlog_config_path");
	if (path_item.type != TOML_STRING) {
		return CONFIG_ERR_SCHEMA;
	}
	path_text = path_item.u.s;
	if (!path_text || strlen(path_text) == 0) {
		return CONFIG_ERR_SCHEMA;
	}

	path_copy = dup_string(path_text);
	if (!path_copy) {
		return CONFIG_ERR_OOM;
	}

	*out_path_copy = path_copy;
	return CONFIG_OK;
}

static int parse_toml_config(struct app_config *cfg, const char *toml_text)
{
	toml_result_t result;
	toml_datum_t root;
	unsigned int interval_seconds = 0;
	char *zlog_config_path = NULL;
	int ret;

	result = toml_parse(toml_text, (int)strlen(toml_text));
	if (!result.ok) {
		toml_free(result);
		return CONFIG_ERR_PARSE;
	}

	root = result.toptab;
	if (root.type != TOML_TABLE || !has_only_known_members(root)) {
		toml_free(result);
		return CONFIG_ERR_SCHEMA;
	}

	ret = parse_interval_seconds(root, &interval_seconds);
	if (ret != CONFIG_OK) {
		toml_free(result);
		return ret;
	}

	ret = parse_zlog_config_path(root, &zlog_config_path);
	if (ret != CONFIG_OK) {
		toml_free(result);
		return ret;
	}

	cfg->interval_seconds = interval_seconds;
	cfg->zlog_config_path = zlog_config_path;

	toml_free(result);
	return CONFIG_OK;
}

static struct config_load_result config_error_result(int error_code)
{
	struct config_load_result result;

	memset(&result.config, 0, sizeof(result.config));
	result.error_code = error_code;
	return result;
}

struct config_load_result config_load_file(const char *path)
{
	FILE *fp;
	char *toml_text;
	int too_large = 0;
	struct config_load_result result = config_error_result(0);

	if (!path) {
		return config_error_result(CONFIG_ERR_INVALID_ARGUMENT);
	}

	fp = fopen(path, "re");
	if (!fp) {
		return config_error_result(CONFIG_ERR_IO);
	}
	toml_text = read_all(fp, &too_large);
	fclose(fp);
	if (!toml_text) {
		if (too_large) {
			return config_error_result(CONFIG_ERR_TOO_LARGE);
		}
		return config_error_result(CONFIG_ERR_IO);
	}

	result.error_code = parse_toml_config(&result.config, toml_text);
	if (result.error_code != CONFIG_OK) {
		free(toml_text);
		config_free(&result.config);
		return result;
	}

	free(toml_text);
	return result;
}

void config_free(struct app_config *cfg)
{
	if (!cfg) {
		return;
	}
	free(cfg->zlog_config_path);
	cfg->zlog_config_path = NULL;
	cfg->interval_seconds = 0;
}
