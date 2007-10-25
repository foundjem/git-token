begin_unit
begin_include
include|#
directive|include
file|"http.h"
end_include
begin_decl_stmt
DECL|variable|data_received
name|int
name|data_received
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|active_requests
name|int
name|active_requests
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_ifdef
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
end_ifdef
begin_decl_stmt
DECL|variable|max_requests
name|int
name|max_requests
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|curlm
name|CURLM
modifier|*
name|curlm
decl_stmt|;
end_decl_stmt
begin_endif
endif|#
directive|endif
end_endif
begin_ifndef
ifndef|#
directive|ifndef
name|NO_CURL_EASY_DUPHANDLE
end_ifndef
begin_decl_stmt
DECL|variable|curl_default
name|CURL
modifier|*
name|curl_default
decl_stmt|;
end_decl_stmt
begin_endif
endif|#
directive|endif
end_endif
begin_decl_stmt
DECL|variable|curl_errorstr
name|char
name|curl_errorstr
index|[
name|CURL_ERROR_SIZE
index|]
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|curl_ssl_verify
name|int
name|curl_ssl_verify
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|ssl_cert
name|char
modifier|*
name|ssl_cert
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_if
if|#
directive|if
name|LIBCURL_VERSION_NUM
operator|>=
literal|0x070902
end_if
begin_decl_stmt
DECL|variable|ssl_key
name|char
modifier|*
name|ssl_key
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_endif
endif|#
directive|endif
end_endif
begin_if
if|#
directive|if
name|LIBCURL_VERSION_NUM
operator|>=
literal|0x070908
end_if
begin_decl_stmt
DECL|variable|ssl_capath
name|char
modifier|*
name|ssl_capath
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_endif
endif|#
directive|endif
end_endif
begin_decl_stmt
DECL|variable|ssl_cainfo
name|char
modifier|*
name|ssl_cainfo
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|curl_low_speed_limit
name|long
name|curl_low_speed_limit
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|curl_low_speed_time
name|long
name|curl_low_speed_time
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|curl_ftp_no_epsv
name|int
name|curl_ftp_no_epsv
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pragma_header
name|struct
name|curl_slist
modifier|*
name|pragma_header
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|active_queue_head
name|struct
name|active_request_slot
modifier|*
name|active_queue_head
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|fread_buffer
name|size_t
name|fread_buffer
parameter_list|(
name|void
modifier|*
name|ptr
parameter_list|,
name|size_t
name|eltsize
parameter_list|,
name|size_t
name|nmemb
parameter_list|,
name|struct
name|buffer
modifier|*
name|buffer
parameter_list|)
block|{
name|size_t
name|size
init|=
name|eltsize
operator|*
name|nmemb
decl_stmt|;
if|if
condition|(
name|size
operator|>
name|buffer
operator|->
name|size
operator|-
name|buffer
operator|->
name|posn
condition|)
name|size
operator|=
name|buffer
operator|->
name|size
operator|-
name|buffer
operator|->
name|posn
expr_stmt|;
name|memcpy
argument_list|(
name|ptr
argument_list|,
operator|(
name|char
operator|*
operator|)
name|buffer
operator|->
name|buffer
operator|+
name|buffer
operator|->
name|posn
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|buffer
operator|->
name|posn
operator|+=
name|size
expr_stmt|;
return|return
name|size
return|;
block|}
end_function
begin_function
DECL|function|fwrite_buffer
name|size_t
name|fwrite_buffer
parameter_list|(
specifier|const
name|void
modifier|*
name|ptr
parameter_list|,
name|size_t
name|eltsize
parameter_list|,
name|size_t
name|nmemb
parameter_list|,
name|struct
name|buffer
modifier|*
name|buffer
parameter_list|)
block|{
name|size_t
name|size
init|=
name|eltsize
operator|*
name|nmemb
decl_stmt|;
if|if
condition|(
name|size
operator|>
name|buffer
operator|->
name|size
operator|-
name|buffer
operator|->
name|posn
condition|)
block|{
name|buffer
operator|->
name|size
operator|=
name|buffer
operator|->
name|size
operator|*
literal|3
operator|/
literal|2
expr_stmt|;
if|if
condition|(
name|buffer
operator|->
name|size
operator|<
name|buffer
operator|->
name|posn
operator|+
name|size
condition|)
name|buffer
operator|->
name|size
operator|=
name|buffer
operator|->
name|posn
operator|+
name|size
expr_stmt|;
name|buffer
operator|->
name|buffer
operator|=
name|xrealloc
argument_list|(
name|buffer
operator|->
name|buffer
argument_list|,
name|buffer
operator|->
name|size
argument_list|)
expr_stmt|;
block|}
name|memcpy
argument_list|(
operator|(
name|char
operator|*
operator|)
name|buffer
operator|->
name|buffer
operator|+
name|buffer
operator|->
name|posn
argument_list|,
name|ptr
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|buffer
operator|->
name|posn
operator|+=
name|size
expr_stmt|;
name|data_received
operator|++
expr_stmt|;
return|return
name|size
return|;
block|}
end_function
begin_function
DECL|function|fwrite_null
name|size_t
name|fwrite_null
parameter_list|(
specifier|const
name|void
modifier|*
name|ptr
parameter_list|,
name|size_t
name|eltsize
parameter_list|,
name|size_t
name|nmemb
parameter_list|,
name|struct
name|buffer
modifier|*
name|buffer
parameter_list|)
block|{
name|data_received
operator|++
expr_stmt|;
return|return
name|eltsize
operator|*
name|nmemb
return|;
block|}
end_function
begin_function_decl
specifier|static
name|void
name|finish_active_slot
parameter_list|(
name|struct
name|active_request_slot
modifier|*
name|slot
parameter_list|)
function_decl|;
end_function_decl
begin_ifdef
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
end_ifdef
begin_function
DECL|function|process_curl_messages
specifier|static
name|void
name|process_curl_messages
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|num_messages
decl_stmt|;
name|struct
name|active_request_slot
modifier|*
name|slot
decl_stmt|;
name|CURLMsg
modifier|*
name|curl_message
init|=
name|curl_multi_info_read
argument_list|(
name|curlm
argument_list|,
operator|&
name|num_messages
argument_list|)
decl_stmt|;
while|while
condition|(
name|curl_message
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
name|curl_message
operator|->
name|msg
operator|==
name|CURLMSG_DONE
condition|)
block|{
name|int
name|curl_result
init|=
name|curl_message
operator|->
name|data
operator|.
name|result
decl_stmt|;
name|slot
operator|=
name|active_queue_head
expr_stmt|;
while|while
condition|(
name|slot
operator|!=
name|NULL
operator|&&
name|slot
operator|->
name|curl
operator|!=
name|curl_message
operator|->
name|easy_handle
condition|)
name|slot
operator|=
name|slot
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|slot
operator|!=
name|NULL
condition|)
block|{
name|curl_multi_remove_handle
argument_list|(
name|curlm
argument_list|,
name|slot
operator|->
name|curl
argument_list|)
expr_stmt|;
name|slot
operator|->
name|curl_result
operator|=
name|curl_result
expr_stmt|;
name|finish_active_slot
argument_list|(
name|slot
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Received DONE message for unknown request!\n"
argument_list|)
expr_stmt|;
block|}
block|}
else|else
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Unknown CURL message received: %d\n"
argument_list|,
operator|(
name|int
operator|)
name|curl_message
operator|->
name|msg
argument_list|)
expr_stmt|;
block|}
name|curl_message
operator|=
name|curl_multi_info_read
argument_list|(
name|curlm
argument_list|,
operator|&
name|num_messages
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|http_options
specifier|static
name|int
name|http_options
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"http.sslverify"
argument_list|,
name|var
argument_list|)
condition|)
block|{
if|if
condition|(
name|curl_ssl_verify
operator|==
operator|-
literal|1
condition|)
block|{
name|curl_ssl_verify
operator|=
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"http.sslcert"
argument_list|,
name|var
argument_list|)
condition|)
block|{
if|if
condition|(
name|ssl_cert
operator|==
name|NULL
condition|)
block|{
name|ssl_cert
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|value
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|ssl_cert
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
if|#
directive|if
name|LIBCURL_VERSION_NUM
operator|>=
literal|0x070902
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"http.sslkey"
argument_list|,
name|var
argument_list|)
condition|)
block|{
if|if
condition|(
name|ssl_key
operator|==
name|NULL
condition|)
block|{
name|ssl_key
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|value
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|ssl_key
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
endif|#
directive|endif
if|#
directive|if
name|LIBCURL_VERSION_NUM
operator|>=
literal|0x070908
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"http.sslcapath"
argument_list|,
name|var
argument_list|)
condition|)
block|{
if|if
condition|(
name|ssl_capath
operator|==
name|NULL
condition|)
block|{
name|ssl_capath
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|value
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|ssl_capath
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
endif|#
directive|endif
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"http.sslcainfo"
argument_list|,
name|var
argument_list|)
condition|)
block|{
if|if
condition|(
name|ssl_cainfo
operator|==
name|NULL
condition|)
block|{
name|ssl_cainfo
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|value
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|ssl_cainfo
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"http.maxrequests"
argument_list|,
name|var
argument_list|)
condition|)
block|{
if|if
condition|(
name|max_requests
operator|==
operator|-
literal|1
condition|)
name|max_requests
operator|=
name|git_config_int
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
endif|#
directive|endif
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"http.lowspeedlimit"
argument_list|,
name|var
argument_list|)
condition|)
block|{
if|if
condition|(
name|curl_low_speed_limit
operator|==
operator|-
literal|1
condition|)
name|curl_low_speed_limit
operator|=
operator|(
name|long
operator|)
name|git_config_int
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"http.lowspeedtime"
argument_list|,
name|var
argument_list|)
condition|)
block|{
if|if
condition|(
name|curl_low_speed_time
operator|==
operator|-
literal|1
condition|)
name|curl_low_speed_time
operator|=
operator|(
name|long
operator|)
name|git_config_int
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"http.noepsv"
argument_list|,
name|var
argument_list|)
condition|)
block|{
name|curl_ftp_no_epsv
operator|=
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* Fall back on the default ones */
return|return
name|git_default_config
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|get_curl_handle
specifier|static
name|CURL
modifier|*
name|get_curl_handle
parameter_list|(
name|void
parameter_list|)
block|{
name|CURL
modifier|*
name|result
init|=
name|curl_easy_init
argument_list|()
decl_stmt|;
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_SSL_VERIFYPEER
argument_list|,
name|curl_ssl_verify
argument_list|)
expr_stmt|;
if|#
directive|if
name|LIBCURL_VERSION_NUM
operator|>=
literal|0x070907
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_NETRC
argument_list|,
name|CURL_NETRC_OPTIONAL
argument_list|)
expr_stmt|;
endif|#
directive|endif
if|if
condition|(
name|ssl_cert
operator|!=
name|NULL
condition|)
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_SSLCERT
argument_list|,
name|ssl_cert
argument_list|)
expr_stmt|;
if|#
directive|if
name|LIBCURL_VERSION_NUM
operator|>=
literal|0x070902
if|if
condition|(
name|ssl_key
operator|!=
name|NULL
condition|)
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_SSLKEY
argument_list|,
name|ssl_key
argument_list|)
expr_stmt|;
endif|#
directive|endif
if|#
directive|if
name|LIBCURL_VERSION_NUM
operator|>=
literal|0x070908
if|if
condition|(
name|ssl_capath
operator|!=
name|NULL
condition|)
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_CAPATH
argument_list|,
name|ssl_capath
argument_list|)
expr_stmt|;
endif|#
directive|endif
if|if
condition|(
name|ssl_cainfo
operator|!=
name|NULL
condition|)
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_CAINFO
argument_list|,
name|ssl_cainfo
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_FAILONERROR
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|curl_low_speed_limit
operator|>
literal|0
operator|&&
name|curl_low_speed_time
operator|>
literal|0
condition|)
block|{
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_LOW_SPEED_LIMIT
argument_list|,
name|curl_low_speed_limit
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_LOW_SPEED_TIME
argument_list|,
name|curl_low_speed_time
argument_list|)
expr_stmt|;
block|}
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_FOLLOWLOCATION
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_CURL_VERBOSE"
argument_list|)
condition|)
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_VERBOSE
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_USERAGENT
argument_list|,
name|GIT_USER_AGENT
argument_list|)
expr_stmt|;
if|if
condition|(
name|curl_ftp_no_epsv
condition|)
name|curl_easy_setopt
argument_list|(
name|result
argument_list|,
name|CURLOPT_FTP_USE_EPSV
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|http_init
name|void
name|http_init
parameter_list|(
name|void
parameter_list|)
block|{
name|char
modifier|*
name|low_speed_limit
decl_stmt|;
name|char
modifier|*
name|low_speed_time
decl_stmt|;
name|curl_global_init
argument_list|(
name|CURL_GLOBAL_ALL
argument_list|)
expr_stmt|;
name|pragma_header
operator|=
name|curl_slist_append
argument_list|(
name|pragma_header
argument_list|,
literal|"Pragma: no-cache"
argument_list|)
expr_stmt|;
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
block|{
name|char
modifier|*
name|http_max_requests
init|=
name|getenv
argument_list|(
literal|"GIT_HTTP_MAX_REQUESTS"
argument_list|)
decl_stmt|;
if|if
condition|(
name|http_max_requests
operator|!=
name|NULL
condition|)
name|max_requests
operator|=
name|atoi
argument_list|(
name|http_max_requests
argument_list|)
expr_stmt|;
block|}
name|curlm
operator|=
name|curl_multi_init
argument_list|()
expr_stmt|;
if|if
condition|(
name|curlm
operator|==
name|NULL
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Error creating curl multi handle.\n"
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
endif|#
directive|endif
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_SSL_NO_VERIFY"
argument_list|)
condition|)
name|curl_ssl_verify
operator|=
literal|0
expr_stmt|;
name|ssl_cert
operator|=
name|getenv
argument_list|(
literal|"GIT_SSL_CERT"
argument_list|)
expr_stmt|;
if|#
directive|if
name|LIBCURL_VERSION_NUM
operator|>=
literal|0x070902
name|ssl_key
operator|=
name|getenv
argument_list|(
literal|"GIT_SSL_KEY"
argument_list|)
expr_stmt|;
endif|#
directive|endif
if|#
directive|if
name|LIBCURL_VERSION_NUM
operator|>=
literal|0x070908
name|ssl_capath
operator|=
name|getenv
argument_list|(
literal|"GIT_SSL_CAPATH"
argument_list|)
expr_stmt|;
endif|#
directive|endif
name|ssl_cainfo
operator|=
name|getenv
argument_list|(
literal|"GIT_SSL_CAINFO"
argument_list|)
expr_stmt|;
name|low_speed_limit
operator|=
name|getenv
argument_list|(
literal|"GIT_HTTP_LOW_SPEED_LIMIT"
argument_list|)
expr_stmt|;
if|if
condition|(
name|low_speed_limit
operator|!=
name|NULL
condition|)
name|curl_low_speed_limit
operator|=
name|strtol
argument_list|(
name|low_speed_limit
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
name|low_speed_time
operator|=
name|getenv
argument_list|(
literal|"GIT_HTTP_LOW_SPEED_TIME"
argument_list|)
expr_stmt|;
if|if
condition|(
name|low_speed_time
operator|!=
name|NULL
condition|)
name|curl_low_speed_time
operator|=
name|strtol
argument_list|(
name|low_speed_time
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
name|git_config
argument_list|(
name|http_options
argument_list|)
expr_stmt|;
if|if
condition|(
name|curl_ssl_verify
operator|==
operator|-
literal|1
condition|)
name|curl_ssl_verify
operator|=
literal|1
expr_stmt|;
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
if|if
condition|(
name|max_requests
operator|<
literal|1
condition|)
name|max_requests
operator|=
name|DEFAULT_MAX_REQUESTS
expr_stmt|;
endif|#
directive|endif
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_CURL_FTP_NO_EPSV"
argument_list|)
condition|)
name|curl_ftp_no_epsv
operator|=
literal|1
expr_stmt|;
ifndef|#
directive|ifndef
name|NO_CURL_EASY_DUPHANDLE
name|curl_default
operator|=
name|get_curl_handle
argument_list|()
expr_stmt|;
endif|#
directive|endif
block|}
end_function
begin_function
DECL|function|http_cleanup
name|void
name|http_cleanup
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|active_request_slot
modifier|*
name|slot
init|=
name|active_queue_head
decl_stmt|;
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
name|char
modifier|*
name|wait_url
decl_stmt|;
endif|#
directive|endif
while|while
condition|(
name|slot
operator|!=
name|NULL
condition|)
block|{
name|struct
name|active_request_slot
modifier|*
name|next
init|=
name|slot
operator|->
name|next
decl_stmt|;
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
if|if
condition|(
name|slot
operator|->
name|in_use
condition|)
block|{
name|curl_easy_getinfo
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLINFO_EFFECTIVE_URL
argument_list|,
operator|&
name|wait_url
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Waiting for %s\n"
argument_list|,
name|wait_url
argument_list|)
expr_stmt|;
name|run_active_slot
argument_list|(
name|slot
argument_list|)
expr_stmt|;
block|}
endif|#
directive|endif
if|if
condition|(
name|slot
operator|->
name|curl
operator|!=
name|NULL
condition|)
name|curl_easy_cleanup
argument_list|(
name|slot
operator|->
name|curl
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|slot
argument_list|)
expr_stmt|;
name|slot
operator|=
name|next
expr_stmt|;
block|}
name|active_queue_head
operator|=
name|NULL
expr_stmt|;
ifndef|#
directive|ifndef
name|NO_CURL_EASY_DUPHANDLE
name|curl_easy_cleanup
argument_list|(
name|curl_default
argument_list|)
expr_stmt|;
endif|#
directive|endif
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
name|curl_multi_cleanup
argument_list|(
name|curlm
argument_list|)
expr_stmt|;
endif|#
directive|endif
name|curl_global_cleanup
argument_list|()
expr_stmt|;
name|curl_slist_free_all
argument_list|(
name|pragma_header
argument_list|)
expr_stmt|;
name|pragma_header
operator|=
name|NULL
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_active_slot
name|struct
name|active_request_slot
modifier|*
name|get_active_slot
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|active_request_slot
modifier|*
name|slot
init|=
name|active_queue_head
decl_stmt|;
name|struct
name|active_request_slot
modifier|*
name|newslot
decl_stmt|;
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
name|int
name|num_transfers
decl_stmt|;
comment|/* Wait for a slot to open up if the queue is full */
while|while
condition|(
name|active_requests
operator|>=
name|max_requests
condition|)
block|{
name|curl_multi_perform
argument_list|(
name|curlm
argument_list|,
operator|&
name|num_transfers
argument_list|)
expr_stmt|;
if|if
condition|(
name|num_transfers
operator|<
name|active_requests
condition|)
block|{
name|process_curl_messages
argument_list|()
expr_stmt|;
block|}
block|}
endif|#
directive|endif
while|while
condition|(
name|slot
operator|!=
name|NULL
operator|&&
name|slot
operator|->
name|in_use
condition|)
block|{
name|slot
operator|=
name|slot
operator|->
name|next
expr_stmt|;
block|}
if|if
condition|(
name|slot
operator|==
name|NULL
condition|)
block|{
name|newslot
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|newslot
argument_list|)
argument_list|)
expr_stmt|;
name|newslot
operator|->
name|curl
operator|=
name|NULL
expr_stmt|;
name|newslot
operator|->
name|in_use
operator|=
literal|0
expr_stmt|;
name|newslot
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
name|slot
operator|=
name|active_queue_head
expr_stmt|;
if|if
condition|(
name|slot
operator|==
name|NULL
condition|)
block|{
name|active_queue_head
operator|=
name|newslot
expr_stmt|;
block|}
else|else
block|{
while|while
condition|(
name|slot
operator|->
name|next
operator|!=
name|NULL
condition|)
block|{
name|slot
operator|=
name|slot
operator|->
name|next
expr_stmt|;
block|}
name|slot
operator|->
name|next
operator|=
name|newslot
expr_stmt|;
block|}
name|slot
operator|=
name|newslot
expr_stmt|;
block|}
if|if
condition|(
name|slot
operator|->
name|curl
operator|==
name|NULL
condition|)
block|{
ifdef|#
directive|ifdef
name|NO_CURL_EASY_DUPHANDLE
name|slot
operator|->
name|curl
operator|=
name|get_curl_handle
argument_list|()
expr_stmt|;
else|#
directive|else
name|slot
operator|->
name|curl
operator|=
name|curl_easy_duphandle
argument_list|(
name|curl_default
argument_list|)
expr_stmt|;
endif|#
directive|endif
block|}
name|active_requests
operator|++
expr_stmt|;
name|slot
operator|->
name|in_use
operator|=
literal|1
expr_stmt|;
name|slot
operator|->
name|local
operator|=
name|NULL
expr_stmt|;
name|slot
operator|->
name|results
operator|=
name|NULL
expr_stmt|;
name|slot
operator|->
name|finished
operator|=
name|NULL
expr_stmt|;
name|slot
operator|->
name|callback_data
operator|=
name|NULL
expr_stmt|;
name|slot
operator|->
name|callback_func
operator|=
name|NULL
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_HTTPHEADER
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_HTTPHEADER
argument_list|,
name|pragma_header
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_ERRORBUFFER
argument_list|,
name|curl_errorstr
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_CUSTOMREQUEST
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_READFUNCTION
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_WRITEFUNCTION
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_UPLOAD
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_HTTPGET
argument_list|,
literal|1
argument_list|)
expr_stmt|;
return|return
name|slot
return|;
block|}
end_function
begin_function
DECL|function|start_active_slot
name|int
name|start_active_slot
parameter_list|(
name|struct
name|active_request_slot
modifier|*
name|slot
parameter_list|)
block|{
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
name|CURLMcode
name|curlm_result
init|=
name|curl_multi_add_handle
argument_list|(
name|curlm
argument_list|,
name|slot
operator|->
name|curl
argument_list|)
decl_stmt|;
name|int
name|num_transfers
decl_stmt|;
if|if
condition|(
name|curlm_result
operator|!=
name|CURLM_OK
operator|&&
name|curlm_result
operator|!=
name|CURLM_CALL_MULTI_PERFORM
condition|)
block|{
name|active_requests
operator|--
expr_stmt|;
name|slot
operator|->
name|in_use
operator|=
literal|0
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* 	 * We know there must be something to do, since we just added 	 * something. 	 */
name|curl_multi_perform
argument_list|(
name|curlm
argument_list|,
operator|&
name|num_transfers
argument_list|)
expr_stmt|;
endif|#
directive|endif
return|return
literal|1
return|;
block|}
end_function
begin_ifdef
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
end_ifdef
begin_struct
DECL|struct|fill_chain
struct|struct
name|fill_chain
block|{
DECL|member|data
name|void
modifier|*
name|data
decl_stmt|;
DECL|member|fill
name|int
function_decl|(
modifier|*
name|fill
function_decl|)
parameter_list|(
name|void
modifier|*
parameter_list|)
function_decl|;
DECL|member|next
name|struct
name|fill_chain
modifier|*
name|next
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|fill_cfg
specifier|static
name|struct
name|fill_chain
modifier|*
name|fill_cfg
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_fill_function
name|void
name|add_fill_function
parameter_list|(
name|void
modifier|*
name|data
parameter_list|,
name|int
function_decl|(
modifier|*
name|fill
function_decl|)
parameter_list|(
name|void
modifier|*
parameter_list|)
parameter_list|)
block|{
name|struct
name|fill_chain
modifier|*
name|new
init|=
name|malloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|new
argument_list|)
argument_list|)
decl_stmt|;
name|struct
name|fill_chain
modifier|*
modifier|*
name|linkp
init|=
operator|&
name|fill_cfg
decl_stmt|;
name|new
operator|->
name|data
operator|=
name|data
expr_stmt|;
name|new
operator|->
name|fill
operator|=
name|fill
expr_stmt|;
name|new
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
while|while
condition|(
operator|*
name|linkp
condition|)
name|linkp
operator|=
operator|&
operator|(
operator|*
name|linkp
operator|)
operator|->
name|next
expr_stmt|;
operator|*
name|linkp
operator|=
name|new
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fill_active_slots
name|void
name|fill_active_slots
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|active_request_slot
modifier|*
name|slot
init|=
name|active_queue_head
decl_stmt|;
while|while
condition|(
name|active_requests
operator|<
name|max_requests
condition|)
block|{
name|struct
name|fill_chain
modifier|*
name|fill
decl_stmt|;
for|for
control|(
name|fill
operator|=
name|fill_cfg
init|;
name|fill
condition|;
name|fill
operator|=
name|fill
operator|->
name|next
control|)
if|if
condition|(
name|fill
operator|->
name|fill
argument_list|(
name|fill
operator|->
name|data
argument_list|)
condition|)
break|break;
if|if
condition|(
operator|!
name|fill
condition|)
break|break;
block|}
while|while
condition|(
name|slot
operator|!=
name|NULL
condition|)
block|{
if|if
condition|(
operator|!
name|slot
operator|->
name|in_use
operator|&&
name|slot
operator|->
name|curl
operator|!=
name|NULL
condition|)
block|{
name|curl_easy_cleanup
argument_list|(
name|slot
operator|->
name|curl
argument_list|)
expr_stmt|;
name|slot
operator|->
name|curl
operator|=
name|NULL
expr_stmt|;
block|}
name|slot
operator|=
name|slot
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|step_active_slots
name|void
name|step_active_slots
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|num_transfers
decl_stmt|;
name|CURLMcode
name|curlm_result
decl_stmt|;
do|do
block|{
name|curlm_result
operator|=
name|curl_multi_perform
argument_list|(
name|curlm
argument_list|,
operator|&
name|num_transfers
argument_list|)
expr_stmt|;
block|}
do|while
condition|(
name|curlm_result
operator|==
name|CURLM_CALL_MULTI_PERFORM
condition|)
do|;
if|if
condition|(
name|num_transfers
operator|<
name|active_requests
condition|)
block|{
name|process_curl_messages
argument_list|()
expr_stmt|;
name|fill_active_slots
argument_list|()
expr_stmt|;
block|}
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|run_active_slot
name|void
name|run_active_slot
parameter_list|(
name|struct
name|active_request_slot
modifier|*
name|slot
parameter_list|)
block|{
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
name|long
name|last_pos
init|=
literal|0
decl_stmt|;
name|long
name|current_pos
decl_stmt|;
name|fd_set
name|readfds
decl_stmt|;
name|fd_set
name|writefds
decl_stmt|;
name|fd_set
name|excfds
decl_stmt|;
name|int
name|max_fd
decl_stmt|;
name|struct
name|timeval
name|select_timeout
decl_stmt|;
name|int
name|finished
init|=
literal|0
decl_stmt|;
name|slot
operator|->
name|finished
operator|=
operator|&
name|finished
expr_stmt|;
while|while
condition|(
operator|!
name|finished
condition|)
block|{
name|data_received
operator|=
literal|0
expr_stmt|;
name|step_active_slots
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|data_received
operator|&&
name|slot
operator|->
name|local
operator|!=
name|NULL
condition|)
block|{
name|current_pos
operator|=
name|ftell
argument_list|(
name|slot
operator|->
name|local
argument_list|)
expr_stmt|;
if|if
condition|(
name|current_pos
operator|>
name|last_pos
condition|)
name|data_received
operator|++
expr_stmt|;
name|last_pos
operator|=
name|current_pos
expr_stmt|;
block|}
if|if
condition|(
name|slot
operator|->
name|in_use
operator|&&
operator|!
name|data_received
condition|)
block|{
name|max_fd
operator|=
literal|0
expr_stmt|;
name|FD_ZERO
argument_list|(
operator|&
name|readfds
argument_list|)
expr_stmt|;
name|FD_ZERO
argument_list|(
operator|&
name|writefds
argument_list|)
expr_stmt|;
name|FD_ZERO
argument_list|(
operator|&
name|excfds
argument_list|)
expr_stmt|;
name|select_timeout
operator|.
name|tv_sec
operator|=
literal|0
expr_stmt|;
name|select_timeout
operator|.
name|tv_usec
operator|=
literal|50000
expr_stmt|;
name|select
argument_list|(
name|max_fd
argument_list|,
operator|&
name|readfds
argument_list|,
operator|&
name|writefds
argument_list|,
operator|&
name|excfds
argument_list|,
operator|&
name|select_timeout
argument_list|)
expr_stmt|;
block|}
block|}
else|#
directive|else
while|while
condition|(
name|slot
operator|->
name|in_use
condition|)
block|{
name|slot
operator|->
name|curl_result
operator|=
name|curl_easy_perform
argument_list|(
name|slot
operator|->
name|curl
argument_list|)
expr_stmt|;
name|finish_active_slot
argument_list|(
name|slot
argument_list|)
expr_stmt|;
block|}
endif|#
directive|endif
block|}
end_function
begin_function
DECL|function|closedown_active_slot
specifier|static
name|void
name|closedown_active_slot
parameter_list|(
name|struct
name|active_request_slot
modifier|*
name|slot
parameter_list|)
block|{
name|active_requests
operator|--
expr_stmt|;
name|slot
operator|->
name|in_use
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|release_active_slot
name|void
name|release_active_slot
parameter_list|(
name|struct
name|active_request_slot
modifier|*
name|slot
parameter_list|)
block|{
name|closedown_active_slot
argument_list|(
name|slot
argument_list|)
expr_stmt|;
if|if
condition|(
name|slot
operator|->
name|curl
condition|)
block|{
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
name|curl_multi_remove_handle
argument_list|(
name|curlm
argument_list|,
name|slot
operator|->
name|curl
argument_list|)
expr_stmt|;
endif|#
directive|endif
name|curl_easy_cleanup
argument_list|(
name|slot
operator|->
name|curl
argument_list|)
expr_stmt|;
name|slot
operator|->
name|curl
operator|=
name|NULL
expr_stmt|;
block|}
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
name|fill_active_slots
argument_list|()
expr_stmt|;
endif|#
directive|endif
block|}
end_function
begin_function
DECL|function|finish_active_slot
specifier|static
name|void
name|finish_active_slot
parameter_list|(
name|struct
name|active_request_slot
modifier|*
name|slot
parameter_list|)
block|{
name|closedown_active_slot
argument_list|(
name|slot
argument_list|)
expr_stmt|;
name|curl_easy_getinfo
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLINFO_HTTP_CODE
argument_list|,
operator|&
name|slot
operator|->
name|http_code
argument_list|)
expr_stmt|;
if|if
condition|(
name|slot
operator|->
name|finished
operator|!=
name|NULL
condition|)
operator|(
operator|*
name|slot
operator|->
name|finished
operator|)
operator|=
literal|1
expr_stmt|;
comment|/* Store slot results so they can be read after the slot is reused */
if|if
condition|(
name|slot
operator|->
name|results
operator|!=
name|NULL
condition|)
block|{
name|slot
operator|->
name|results
operator|->
name|curl_result
operator|=
name|slot
operator|->
name|curl_result
expr_stmt|;
name|slot
operator|->
name|results
operator|->
name|http_code
operator|=
name|slot
operator|->
name|http_code
expr_stmt|;
block|}
comment|/* Run callback if appropriate */
if|if
condition|(
name|slot
operator|->
name|callback_func
operator|!=
name|NULL
condition|)
block|{
name|slot
operator|->
name|callback_func
argument_list|(
name|slot
operator|->
name|callback_data
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|finish_all_active_slots
name|void
name|finish_all_active_slots
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|active_request_slot
modifier|*
name|slot
init|=
name|active_queue_head
decl_stmt|;
while|while
condition|(
name|slot
operator|!=
name|NULL
condition|)
if|if
condition|(
name|slot
operator|->
name|in_use
condition|)
block|{
name|run_active_slot
argument_list|(
name|slot
argument_list|)
expr_stmt|;
name|slot
operator|=
name|active_queue_head
expr_stmt|;
block|}
else|else
block|{
name|slot
operator|=
name|slot
operator|->
name|next
expr_stmt|;
block|}
block|}
end_function
end_unit
