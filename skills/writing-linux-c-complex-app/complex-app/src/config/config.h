#ifndef _CONFIG_H
#define _CONFIG_H

/*
 * config module features:
 * - loads app configuration from a TOML file
 * - enforces config file size limit: 1MB
 * - validates schema and value constraints during parsing
 * - returns structured data and explicit error codes
 */

/** Parsed application configuration from the TOML main config file. */
struct app_config {
	unsigned int interval_seconds; /**< Main loop interval in seconds (>= 1). */
	char *zlog_config_path;      /**< Path to zlog ini file (heap-owned, non-empty). */
};

/** Error codes returned by config module APIs. */
enum config_error_code {
	CONFIG_OK = 0,                   /**< Success. */
	CONFIG_ERR_INVALID_ARGUMENT = 1, /**< Invalid input argument (e.g. NULL path). */
	CONFIG_ERR_IO = 2,               /**< File open/read error. */
	CONFIG_ERR_PARSE = 3,            /**< TOML parse failed. */
	CONFIG_ERR_OOM = 4,              /**< Memory allocation failed. */
	CONFIG_ERR_SCHEMA = 5,           /**< TOML schema or value constraints invalid. */
	CONFIG_ERR_TOO_LARGE = 6         /**< Config file exceeds 1MB limit. */
};

/** Result of loading a configuration file. */
struct config_load_result {
	int error_code;           /**< One of config_error_code values. */
	struct app_config config; /**< Valid only when error_code is CONFIG_OK. */
};

/**
 * @brief Load configuration from a specified TOML file.
 * @param path Path to the TOML configuration file.
 * @return Load result with error_code and parsed config on success.
 */
struct config_load_result config_load_file(const char *path);

/**
 * @brief Free resources held by an app_config instance.
 * @param cfg Pointer to the app_config to release (may be partially filled).
 */
void config_free(struct app_config *cfg);

#endif /* _CONFIG_H */
