begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"walker.h"
end_include
begin_include
include|#
directive|include
file|"http.h"
end_include
begin_struct
DECL|struct|alt_base
struct|struct
name|alt_base
block|{
DECL|member|base
name|char
modifier|*
name|base
decl_stmt|;
DECL|member|got_indices
name|int
name|got_indices
decl_stmt|;
DECL|member|packs
name|struct
name|packed_git
modifier|*
name|packs
decl_stmt|;
DECL|member|next
name|struct
name|alt_base
modifier|*
name|next
decl_stmt|;
block|}
struct|;
end_struct
begin_enum
DECL|enum|object_request_state
enum|enum
name|object_request_state
block|{
DECL|enumerator|WAITING
name|WAITING
block|,
DECL|enumerator|ABORTED
name|ABORTED
block|,
DECL|enumerator|ACTIVE
name|ACTIVE
block|,
DECL|enumerator|COMPLETE
name|COMPLETE
block|, }
enum|;
end_enum
begin_struct
DECL|struct|object_request
struct|struct
name|object_request
block|{
DECL|member|walker
name|struct
name|walker
modifier|*
name|walker
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|repo
name|struct
name|alt_base
modifier|*
name|repo
decl_stmt|;
DECL|member|state
name|enum
name|object_request_state
name|state
decl_stmt|;
DECL|member|req
name|struct
name|http_object_request
modifier|*
name|req
decl_stmt|;
DECL|member|next
name|struct
name|object_request
modifier|*
name|next
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|alternates_request
struct|struct
name|alternates_request
block|{
DECL|member|walker
name|struct
name|walker
modifier|*
name|walker
decl_stmt|;
DECL|member|base
specifier|const
name|char
modifier|*
name|base
decl_stmt|;
DECL|member|url
name|char
modifier|*
name|url
decl_stmt|;
DECL|member|buffer
name|struct
name|strbuf
modifier|*
name|buffer
decl_stmt|;
DECL|member|slot
name|struct
name|active_request_slot
modifier|*
name|slot
decl_stmt|;
DECL|member|http_specific
name|int
name|http_specific
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|walker_data
struct|struct
name|walker_data
block|{
DECL|member|url
specifier|const
name|char
modifier|*
name|url
decl_stmt|;
DECL|member|got_alternates
name|int
name|got_alternates
decl_stmt|;
DECL|member|alt
name|struct
name|alt_base
modifier|*
name|alt
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|object_queue_head
specifier|static
name|struct
name|object_request
modifier|*
name|object_queue_head
decl_stmt|;
end_decl_stmt
begin_function_decl
specifier|static
name|void
name|fetch_alternates
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
function_decl|;
end_function_decl
begin_function_decl
specifier|static
name|void
name|process_object_response
parameter_list|(
name|void
modifier|*
name|callback_data
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|start_object_request
specifier|static
name|void
name|start_object_request
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|object_request
modifier|*
name|obj_req
parameter_list|)
block|{
name|struct
name|active_request_slot
modifier|*
name|slot
decl_stmt|;
name|struct
name|http_object_request
modifier|*
name|req
decl_stmt|;
name|req
operator|=
name|new_http_object_request
argument_list|(
name|obj_req
operator|->
name|repo
operator|->
name|base
argument_list|,
name|obj_req
operator|->
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|req
operator|==
name|NULL
condition|)
block|{
name|obj_req
operator|->
name|state
operator|=
name|ABORTED
expr_stmt|;
return|return;
block|}
name|obj_req
operator|->
name|req
operator|=
name|req
expr_stmt|;
name|slot
operator|=
name|req
operator|->
name|slot
expr_stmt|;
name|slot
operator|->
name|callback_func
operator|=
name|process_object_response
expr_stmt|;
name|slot
operator|->
name|callback_data
operator|=
name|obj_req
expr_stmt|;
comment|/* Try to get the request started, abort the request on error */
name|obj_req
operator|->
name|state
operator|=
name|ACTIVE
expr_stmt|;
if|if
condition|(
operator|!
name|start_active_slot
argument_list|(
name|slot
argument_list|)
condition|)
block|{
name|obj_req
operator|->
name|state
operator|=
name|ABORTED
expr_stmt|;
name|release_http_object_request
argument_list|(
name|req
argument_list|)
expr_stmt|;
return|return;
block|}
block|}
end_function
begin_function
DECL|function|finish_object_request
specifier|static
name|void
name|finish_object_request
parameter_list|(
name|struct
name|object_request
modifier|*
name|obj_req
parameter_list|)
block|{
if|if
condition|(
name|finish_http_object_request
argument_list|(
name|obj_req
operator|->
name|req
argument_list|)
condition|)
return|return;
if|if
condition|(
name|obj_req
operator|->
name|req
operator|->
name|rename
operator|==
literal|0
condition|)
name|walker_say
argument_list|(
name|obj_req
operator|->
name|walker
argument_list|,
literal|"got %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj_req
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|process_object_response
specifier|static
name|void
name|process_object_response
parameter_list|(
name|void
modifier|*
name|callback_data
parameter_list|)
block|{
name|struct
name|object_request
modifier|*
name|obj_req
init|=
operator|(
expr|struct
name|object_request
operator|*
operator|)
name|callback_data
decl_stmt|;
name|struct
name|walker
modifier|*
name|walker
init|=
name|obj_req
operator|->
name|walker
decl_stmt|;
name|struct
name|walker_data
modifier|*
name|data
init|=
name|walker
operator|->
name|data
decl_stmt|;
name|struct
name|alt_base
modifier|*
name|alt
init|=
name|data
operator|->
name|alt
decl_stmt|;
name|process_http_object_request
argument_list|(
name|obj_req
operator|->
name|req
argument_list|)
expr_stmt|;
name|obj_req
operator|->
name|state
operator|=
name|COMPLETE
expr_stmt|;
comment|/* Use alternates if necessary */
if|if
condition|(
name|missing_target
argument_list|(
name|obj_req
operator|->
name|req
argument_list|)
condition|)
block|{
name|fetch_alternates
argument_list|(
name|walker
argument_list|,
name|alt
operator|->
name|base
argument_list|)
expr_stmt|;
if|if
condition|(
name|obj_req
operator|->
name|repo
operator|->
name|next
operator|!=
name|NULL
condition|)
block|{
name|obj_req
operator|->
name|repo
operator|=
name|obj_req
operator|->
name|repo
operator|->
name|next
expr_stmt|;
name|release_http_object_request
argument_list|(
name|obj_req
operator|->
name|req
argument_list|)
expr_stmt|;
name|start_object_request
argument_list|(
name|walker
argument_list|,
name|obj_req
argument_list|)
expr_stmt|;
return|return;
block|}
block|}
name|finish_object_request
argument_list|(
name|obj_req
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|release_object_request
specifier|static
name|void
name|release_object_request
parameter_list|(
name|struct
name|object_request
modifier|*
name|obj_req
parameter_list|)
block|{
name|struct
name|object_request
modifier|*
name|entry
init|=
name|object_queue_head
decl_stmt|;
if|if
condition|(
name|obj_req
operator|->
name|req
operator|!=
name|NULL
operator|&&
name|obj_req
operator|->
name|req
operator|->
name|localfile
operator|!=
operator|-
literal|1
condition|)
name|error
argument_list|(
literal|"fd leakage in release: %d"
argument_list|,
name|obj_req
operator|->
name|req
operator|->
name|localfile
argument_list|)
expr_stmt|;
if|if
condition|(
name|obj_req
operator|==
name|object_queue_head
condition|)
block|{
name|object_queue_head
operator|=
name|obj_req
operator|->
name|next
expr_stmt|;
block|}
else|else
block|{
while|while
condition|(
name|entry
operator|->
name|next
operator|!=
name|NULL
operator|&&
name|entry
operator|->
name|next
operator|!=
name|obj_req
condition|)
name|entry
operator|=
name|entry
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|entry
operator|->
name|next
operator|==
name|obj_req
condition|)
name|entry
operator|->
name|next
operator|=
name|entry
operator|->
name|next
operator|->
name|next
expr_stmt|;
block|}
name|free
argument_list|(
name|obj_req
argument_list|)
expr_stmt|;
block|}
end_function
begin_ifdef
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
end_ifdef
begin_function
DECL|function|fill_active_slot
specifier|static
name|int
name|fill_active_slot
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|)
block|{
name|struct
name|object_request
modifier|*
name|obj_req
decl_stmt|;
for|for
control|(
name|obj_req
operator|=
name|object_queue_head
init|;
name|obj_req
condition|;
name|obj_req
operator|=
name|obj_req
operator|->
name|next
control|)
block|{
if|if
condition|(
name|obj_req
operator|->
name|state
operator|==
name|WAITING
condition|)
block|{
if|if
condition|(
name|has_sha1_file
argument_list|(
name|obj_req
operator|->
name|sha1
argument_list|)
condition|)
name|obj_req
operator|->
name|state
operator|=
name|COMPLETE
expr_stmt|;
else|else
block|{
name|start_object_request
argument_list|(
name|walker
argument_list|,
name|obj_req
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|prefetch
specifier|static
name|void
name|prefetch
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object_request
modifier|*
name|newreq
decl_stmt|;
name|struct
name|object_request
modifier|*
name|tail
decl_stmt|;
name|struct
name|walker_data
modifier|*
name|data
init|=
name|walker
operator|->
name|data
decl_stmt|;
name|newreq
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|newreq
argument_list|)
argument_list|)
expr_stmt|;
name|newreq
operator|->
name|walker
operator|=
name|walker
expr_stmt|;
name|hashcpy
argument_list|(
name|newreq
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|newreq
operator|->
name|repo
operator|=
name|data
operator|->
name|alt
expr_stmt|;
name|newreq
operator|->
name|state
operator|=
name|WAITING
expr_stmt|;
name|newreq
operator|->
name|req
operator|=
name|NULL
expr_stmt|;
name|newreq
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
name|http_is_verbose
operator|=
name|walker
operator|->
name|get_verbosely
expr_stmt|;
if|if
condition|(
name|object_queue_head
operator|==
name|NULL
condition|)
block|{
name|object_queue_head
operator|=
name|newreq
expr_stmt|;
block|}
else|else
block|{
name|tail
operator|=
name|object_queue_head
expr_stmt|;
while|while
condition|(
name|tail
operator|->
name|next
operator|!=
name|NULL
condition|)
name|tail
operator|=
name|tail
operator|->
name|next
expr_stmt|;
name|tail
operator|->
name|next
operator|=
name|newreq
expr_stmt|;
block|}
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
name|fill_active_slots
argument_list|()
expr_stmt|;
name|step_active_slots
argument_list|()
expr_stmt|;
endif|#
directive|endif
block|}
end_function
begin_function
DECL|function|process_alternates_response
specifier|static
name|void
name|process_alternates_response
parameter_list|(
name|void
modifier|*
name|callback_data
parameter_list|)
block|{
name|struct
name|alternates_request
modifier|*
name|alt_req
init|=
operator|(
expr|struct
name|alternates_request
operator|*
operator|)
name|callback_data
decl_stmt|;
name|struct
name|walker
modifier|*
name|walker
init|=
name|alt_req
operator|->
name|walker
decl_stmt|;
name|struct
name|walker_data
modifier|*
name|cdata
init|=
name|walker
operator|->
name|data
decl_stmt|;
name|struct
name|active_request_slot
modifier|*
name|slot
init|=
name|alt_req
operator|->
name|slot
decl_stmt|;
name|struct
name|alt_base
modifier|*
name|tail
init|=
name|cdata
operator|->
name|alt
decl_stmt|;
specifier|const
name|char
modifier|*
name|base
init|=
name|alt_req
operator|->
name|base
decl_stmt|;
specifier|static
specifier|const
name|char
name|null_byte
init|=
literal|'\0'
decl_stmt|;
name|char
modifier|*
name|data
decl_stmt|;
name|int
name|i
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|alt_req
operator|->
name|http_specific
condition|)
block|{
if|if
condition|(
name|slot
operator|->
name|curl_result
operator|!=
name|CURLE_OK
operator|||
operator|!
name|alt_req
operator|->
name|buffer
operator|->
name|len
condition|)
block|{
comment|/* Try reusing the slot to get non-http alternates */
name|alt_req
operator|->
name|http_specific
operator|=
literal|0
expr_stmt|;
name|sprintf
argument_list|(
name|alt_req
operator|->
name|url
argument_list|,
literal|"%s/objects/info/alternates"
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_URL
argument_list|,
name|alt_req
operator|->
name|url
argument_list|)
expr_stmt|;
name|active_requests
operator|++
expr_stmt|;
name|slot
operator|->
name|in_use
operator|=
literal|1
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
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|start_active_slot
argument_list|(
name|slot
argument_list|)
condition|)
block|{
name|cdata
operator|->
name|got_alternates
operator|=
operator|-
literal|1
expr_stmt|;
name|slot
operator|->
name|in_use
operator|=
literal|0
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
block|}
return|return;
block|}
block|}
elseif|else
if|if
condition|(
name|slot
operator|->
name|curl_result
operator|!=
name|CURLE_OK
condition|)
block|{
if|if
condition|(
operator|!
name|missing_target
argument_list|(
name|slot
argument_list|)
condition|)
block|{
name|cdata
operator|->
name|got_alternates
operator|=
operator|-
literal|1
expr_stmt|;
return|return;
block|}
block|}
name|fwrite_buffer
argument_list|(
operator|&
name|null_byte
argument_list|,
literal|1
argument_list|,
literal|1
argument_list|,
name|alt_req
operator|->
name|buffer
argument_list|)
expr_stmt|;
name|alt_req
operator|->
name|buffer
operator|->
name|len
operator|--
expr_stmt|;
name|data
operator|=
name|alt_req
operator|->
name|buffer
operator|->
name|buf
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|alt_req
operator|->
name|buffer
operator|->
name|len
condition|)
block|{
name|int
name|posn
init|=
name|i
decl_stmt|;
while|while
condition|(
name|posn
operator|<
name|alt_req
operator|->
name|buffer
operator|->
name|len
operator|&&
name|data
index|[
name|posn
index|]
operator|!=
literal|'\n'
condition|)
name|posn
operator|++
expr_stmt|;
if|if
condition|(
name|data
index|[
name|posn
index|]
operator|==
literal|'\n'
condition|)
block|{
name|int
name|okay
init|=
literal|0
decl_stmt|;
name|int
name|serverlen
init|=
literal|0
decl_stmt|;
name|struct
name|alt_base
modifier|*
name|newalt
decl_stmt|;
name|char
modifier|*
name|target
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|data
index|[
name|i
index|]
operator|==
literal|'/'
condition|)
block|{
comment|/* 				 * This counts 				 * http://git.host/pub/scm/linux.git/ 				 * -----------here^ 				 * so memcpy(dst, base, serverlen) will 				 * copy up to "...git.host". 				 */
specifier|const
name|char
modifier|*
name|colon_ss
init|=
name|strstr
argument_list|(
name|base
argument_list|,
literal|"://"
argument_list|)
decl_stmt|;
if|if
condition|(
name|colon_ss
condition|)
block|{
name|serverlen
operator|=
operator|(
name|strchr
argument_list|(
name|colon_ss
operator|+
literal|3
argument_list|,
literal|'/'
argument_list|)
operator|-
name|base
operator|)
expr_stmt|;
name|okay
operator|=
literal|1
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|data
operator|+
name|i
argument_list|,
literal|"../"
argument_list|,
literal|3
argument_list|)
condition|)
block|{
comment|/* 				 * Relative URL; chop the corresponding 				 * number of subpath from base (and ../ 				 * from data), and concatenate the result. 				 * 				 * The code first drops ../ from data, and 				 * then drops one ../ from data and one path 				 * from base.  IOW, one extra ../ is dropped 				 * from data than path is dropped from base. 				 * 				 * This is not wrong.  The alternate in 				 *     http://git.host/pub/scm/linux.git/ 				 * to borrow from 				 *     http://git.host/pub/scm/linus.git/ 				 * is ../../linus.git/objects/.  You need 				 * two ../../ to borrow from your direct 				 * neighbour. 				 */
name|i
operator|+=
literal|3
expr_stmt|;
name|serverlen
operator|=
name|strlen
argument_list|(
name|base
argument_list|)
expr_stmt|;
while|while
condition|(
name|i
operator|+
literal|2
operator|<
name|posn
operator|&&
operator|!
name|memcmp
argument_list|(
name|data
operator|+
name|i
argument_list|,
literal|"../"
argument_list|,
literal|3
argument_list|)
condition|)
block|{
do|do
block|{
name|serverlen
operator|--
expr_stmt|;
block|}
do|while
condition|(
name|serverlen
operator|&&
name|base
index|[
name|serverlen
operator|-
literal|1
index|]
operator|!=
literal|'/'
condition|)
do|;
name|i
operator|+=
literal|3
expr_stmt|;
block|}
comment|/* If the server got removed, give up. */
name|okay
operator|=
name|strchr
argument_list|(
name|base
argument_list|,
literal|':'
argument_list|)
operator|-
name|base
operator|+
literal|3
operator|<
name|serverlen
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|alt_req
operator|->
name|http_specific
condition|)
block|{
name|char
modifier|*
name|colon
init|=
name|strchr
argument_list|(
name|data
operator|+
name|i
argument_list|,
literal|':'
argument_list|)
decl_stmt|;
name|char
modifier|*
name|slash
init|=
name|strchr
argument_list|(
name|data
operator|+
name|i
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
if|if
condition|(
name|colon
operator|&&
name|slash
operator|&&
name|colon
operator|<
name|data
operator|+
name|posn
operator|&&
name|slash
operator|<
name|data
operator|+
name|posn
operator|&&
name|colon
operator|<
name|slash
condition|)
block|{
name|okay
operator|=
literal|1
expr_stmt|;
block|}
block|}
comment|/* skip "objects\n" at end */
if|if
condition|(
name|okay
condition|)
block|{
name|target
operator|=
name|xmalloc
argument_list|(
name|serverlen
operator|+
name|posn
operator|-
name|i
operator|-
literal|6
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|target
argument_list|,
name|base
argument_list|,
name|serverlen
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|target
operator|+
name|serverlen
argument_list|,
name|data
operator|+
name|i
argument_list|,
name|posn
operator|-
name|i
operator|-
literal|7
argument_list|)
expr_stmt|;
name|target
index|[
name|serverlen
operator|+
name|posn
operator|-
name|i
operator|-
literal|7
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|walker
operator|->
name|get_verbosely
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Also look at %s\n"
argument_list|,
name|target
argument_list|)
expr_stmt|;
name|newalt
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|newalt
argument_list|)
argument_list|)
expr_stmt|;
name|newalt
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
name|newalt
operator|->
name|base
operator|=
name|target
expr_stmt|;
name|newalt
operator|->
name|got_indices
operator|=
literal|0
expr_stmt|;
name|newalt
operator|->
name|packs
operator|=
name|NULL
expr_stmt|;
while|while
condition|(
name|tail
operator|->
name|next
operator|!=
name|NULL
condition|)
name|tail
operator|=
name|tail
operator|->
name|next
expr_stmt|;
name|tail
operator|->
name|next
operator|=
name|newalt
expr_stmt|;
block|}
block|}
name|i
operator|=
name|posn
operator|+
literal|1
expr_stmt|;
block|}
name|cdata
operator|->
name|got_alternates
operator|=
literal|1
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fetch_alternates
specifier|static
name|void
name|fetch_alternates
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|struct
name|strbuf
name|buffer
init|=
name|STRBUF_INIT
decl_stmt|;
name|char
modifier|*
name|url
decl_stmt|;
name|struct
name|active_request_slot
modifier|*
name|slot
decl_stmt|;
name|struct
name|alternates_request
name|alt_req
decl_stmt|;
name|struct
name|walker_data
modifier|*
name|cdata
init|=
name|walker
operator|->
name|data
decl_stmt|;
comment|/* 	 * If another request has already started fetching alternates, 	 * wait for them to arrive and return to processing this request's 	 * curl message 	 */
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
while|while
condition|(
name|cdata
operator|->
name|got_alternates
operator|==
literal|0
condition|)
block|{
name|step_active_slots
argument_list|()
expr_stmt|;
block|}
endif|#
directive|endif
comment|/* Nothing to do if they've already been fetched */
if|if
condition|(
name|cdata
operator|->
name|got_alternates
operator|==
literal|1
condition|)
return|return;
comment|/* Start the fetch */
name|cdata
operator|->
name|got_alternates
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|walker
operator|->
name|get_verbosely
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Getting alternates list for %s\n"
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|url
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|base
argument_list|)
operator|+
literal|31
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|url
argument_list|,
literal|"%s/objects/info/http-alternates"
argument_list|,
name|base
argument_list|)
expr_stmt|;
comment|/* 	 * Use a callback to process the result, since another request 	 * may fail and need to have alternates loaded before continuing 	 */
name|slot
operator|=
name|get_active_slot
argument_list|()
expr_stmt|;
name|slot
operator|->
name|callback_func
operator|=
name|process_alternates_response
expr_stmt|;
name|alt_req
operator|.
name|walker
operator|=
name|walker
expr_stmt|;
name|slot
operator|->
name|callback_data
operator|=
operator|&
name|alt_req
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_FILE
argument_list|,
operator|&
name|buffer
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
name|fwrite_buffer
argument_list|)
expr_stmt|;
name|curl_easy_setopt
argument_list|(
name|slot
operator|->
name|curl
argument_list|,
name|CURLOPT_URL
argument_list|,
name|url
argument_list|)
expr_stmt|;
name|alt_req
operator|.
name|base
operator|=
name|base
expr_stmt|;
name|alt_req
operator|.
name|url
operator|=
name|url
expr_stmt|;
name|alt_req
operator|.
name|buffer
operator|=
operator|&
name|buffer
expr_stmt|;
name|alt_req
operator|.
name|http_specific
operator|=
literal|1
expr_stmt|;
name|alt_req
operator|.
name|slot
operator|=
name|slot
expr_stmt|;
if|if
condition|(
name|start_active_slot
argument_list|(
name|slot
argument_list|)
condition|)
name|run_active_slot
argument_list|(
name|slot
argument_list|)
expr_stmt|;
else|else
name|cdata
operator|->
name|got_alternates
operator|=
operator|-
literal|1
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buffer
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|url
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fetch_indices
specifier|static
name|int
name|fetch_indices
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|alt_base
modifier|*
name|repo
parameter_list|)
block|{
name|int
name|ret
decl_stmt|;
if|if
condition|(
name|repo
operator|->
name|got_indices
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|walker
operator|->
name|get_verbosely
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Getting pack list for %s\n"
argument_list|,
name|repo
operator|->
name|base
argument_list|)
expr_stmt|;
switch|switch
condition|(
name|http_get_info_packs
argument_list|(
name|repo
operator|->
name|base
argument_list|,
operator|&
name|repo
operator|->
name|packs
argument_list|)
condition|)
block|{
case|case
name|HTTP_OK
case|:
case|case
name|HTTP_MISSING_TARGET
case|:
name|repo
operator|->
name|got_indices
operator|=
literal|1
expr_stmt|;
name|ret
operator|=
literal|0
expr_stmt|;
break|break;
default|default:
name|repo
operator|->
name|got_indices
operator|=
literal|0
expr_stmt|;
name|ret
operator|=
operator|-
literal|1
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|fetch_pack
specifier|static
name|int
name|fetch_pack
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|alt_base
modifier|*
name|repo
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|packed_git
modifier|*
name|target
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|struct
name|slot_results
name|results
decl_stmt|;
name|struct
name|http_pack_request
modifier|*
name|preq
decl_stmt|;
if|if
condition|(
name|fetch_indices
argument_list|(
name|walker
argument_list|,
name|repo
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|target
operator|=
name|find_sha1_pack
argument_list|(
name|sha1
argument_list|,
name|repo
operator|->
name|packs
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|target
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|walker
operator|->
name|get_verbosely
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Getting pack %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|target
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" which contains %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|preq
operator|=
name|new_http_pack_request
argument_list|(
name|target
argument_list|,
name|repo
operator|->
name|base
argument_list|)
expr_stmt|;
if|if
condition|(
name|preq
operator|==
name|NULL
condition|)
goto|goto
name|abort
goto|;
name|preq
operator|->
name|lst
operator|=
operator|&
name|repo
operator|->
name|packs
expr_stmt|;
name|preq
operator|->
name|slot
operator|->
name|results
operator|=
operator|&
name|results
expr_stmt|;
if|if
condition|(
name|start_active_slot
argument_list|(
name|preq
operator|->
name|slot
argument_list|)
condition|)
block|{
name|run_active_slot
argument_list|(
name|preq
operator|->
name|slot
argument_list|)
expr_stmt|;
if|if
condition|(
name|results
operator|.
name|curl_result
operator|!=
name|CURLE_OK
condition|)
block|{
name|error
argument_list|(
literal|"Unable to get pack file %s\n%s"
argument_list|,
name|preq
operator|->
name|url
argument_list|,
name|curl_errorstr
argument_list|)
expr_stmt|;
goto|goto
name|abort
goto|;
block|}
block|}
else|else
block|{
name|error
argument_list|(
literal|"Unable to start request"
argument_list|)
expr_stmt|;
goto|goto
name|abort
goto|;
block|}
name|ret
operator|=
name|finish_http_pack_request
argument_list|(
name|preq
argument_list|)
expr_stmt|;
name|release_http_pack_request
argument_list|(
name|preq
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
return|return
name|ret
return|;
return|return
literal|0
return|;
name|abort
label|:
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|abort_object_request
specifier|static
name|void
name|abort_object_request
parameter_list|(
name|struct
name|object_request
modifier|*
name|obj_req
parameter_list|)
block|{
name|release_object_request
argument_list|(
name|obj_req
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|fetch_object
specifier|static
name|int
name|fetch_object
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|alt_base
modifier|*
name|repo
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|char
modifier|*
name|hex
init|=
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|int
name|ret
init|=
literal|0
decl_stmt|;
name|struct
name|object_request
modifier|*
name|obj_req
init|=
name|object_queue_head
decl_stmt|;
name|struct
name|http_object_request
modifier|*
name|req
decl_stmt|;
while|while
condition|(
name|obj_req
operator|!=
name|NULL
operator|&&
name|hashcmp
argument_list|(
name|obj_req
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|)
condition|)
name|obj_req
operator|=
name|obj_req
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|obj_req
operator|==
name|NULL
condition|)
return|return
name|error
argument_list|(
literal|"Couldn't find request for %s in the queue"
argument_list|,
name|hex
argument_list|)
return|;
if|if
condition|(
name|has_sha1_file
argument_list|(
name|obj_req
operator|->
name|sha1
argument_list|)
condition|)
block|{
if|if
condition|(
name|obj_req
operator|->
name|req
operator|!=
name|NULL
condition|)
name|abort_http_object_request
argument_list|(
name|obj_req
operator|->
name|req
argument_list|)
expr_stmt|;
name|abort_object_request
argument_list|(
name|obj_req
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
while|while
condition|(
name|obj_req
operator|->
name|state
operator|==
name|WAITING
condition|)
name|step_active_slots
argument_list|()
expr_stmt|;
else|#
directive|else
name|start_object_request
argument_list|(
name|walker
argument_list|,
name|obj_req
argument_list|)
expr_stmt|;
endif|#
directive|endif
comment|/* 	 * obj_req->req might change when fetching alternates in the callback 	 * process_object_response; therefore, the "shortcut" variable, req, 	 * is used only after we're done with slots. 	 */
while|while
condition|(
name|obj_req
operator|->
name|state
operator|==
name|ACTIVE
condition|)
name|run_active_slot
argument_list|(
name|obj_req
operator|->
name|req
operator|->
name|slot
argument_list|)
expr_stmt|;
name|req
operator|=
name|obj_req
operator|->
name|req
expr_stmt|;
if|if
condition|(
name|req
operator|->
name|localfile
operator|!=
operator|-
literal|1
condition|)
block|{
name|close
argument_list|(
name|req
operator|->
name|localfile
argument_list|)
expr_stmt|;
name|req
operator|->
name|localfile
operator|=
operator|-
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|obj_req
operator|->
name|state
operator|==
name|ABORTED
condition|)
block|{
name|ret
operator|=
name|error
argument_list|(
literal|"Request for %s aborted"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|req
operator|->
name|curl_result
operator|!=
name|CURLE_OK
operator|&&
name|req
operator|->
name|http_code
operator|!=
literal|416
condition|)
block|{
if|if
condition|(
name|missing_target
argument_list|(
name|req
argument_list|)
condition|)
name|ret
operator|=
operator|-
literal|1
expr_stmt|;
comment|/* Be silent, it is probably in a pack. */
else|else
name|ret
operator|=
name|error
argument_list|(
literal|"%s (curl_result = %d, http_code = %ld, sha1 = %s)"
argument_list|,
name|req
operator|->
name|errorstr
argument_list|,
name|req
operator|->
name|curl_result
argument_list|,
name|req
operator|->
name|http_code
argument_list|,
name|hex
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|req
operator|->
name|zret
operator|!=
name|Z_STREAM_END
condition|)
block|{
name|walker
operator|->
name|corrupt_object_found
operator|++
expr_stmt|;
name|ret
operator|=
name|error
argument_list|(
literal|"File %s (%s) corrupt"
argument_list|,
name|hex
argument_list|,
name|req
operator|->
name|url
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|hashcmp
argument_list|(
name|obj_req
operator|->
name|sha1
argument_list|,
name|req
operator|->
name|real_sha1
argument_list|)
condition|)
block|{
name|ret
operator|=
name|error
argument_list|(
literal|"File %s has bad hash"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|req
operator|->
name|rename
operator|<
literal|0
condition|)
block|{
name|ret
operator|=
name|error
argument_list|(
literal|"unable to write sha1 filename %s"
argument_list|,
name|req
operator|->
name|filename
argument_list|)
expr_stmt|;
block|}
name|release_http_object_request
argument_list|(
name|req
argument_list|)
expr_stmt|;
name|release_object_request
argument_list|(
name|obj_req
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|fetch
specifier|static
name|int
name|fetch
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|walker_data
modifier|*
name|data
init|=
name|walker
operator|->
name|data
decl_stmt|;
name|struct
name|alt_base
modifier|*
name|altbase
init|=
name|data
operator|->
name|alt
decl_stmt|;
if|if
condition|(
operator|!
name|fetch_object
argument_list|(
name|walker
argument_list|,
name|altbase
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
while|while
condition|(
name|altbase
condition|)
block|{
if|if
condition|(
operator|!
name|fetch_pack
argument_list|(
name|walker
argument_list|,
name|altbase
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
name|fetch_alternates
argument_list|(
name|walker
argument_list|,
name|data
operator|->
name|alt
operator|->
name|base
argument_list|)
expr_stmt|;
name|altbase
operator|=
name|altbase
operator|->
name|next
expr_stmt|;
block|}
return|return
name|error
argument_list|(
literal|"Unable to find %s under %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|data
operator|->
name|alt
operator|->
name|base
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|fetch_ref
specifier|static
name|int
name|fetch_ref
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
block|{
name|struct
name|walker_data
modifier|*
name|data
init|=
name|walker
operator|->
name|data
decl_stmt|;
return|return
name|http_fetch_ref
argument_list|(
name|data
operator|->
name|alt
operator|->
name|base
argument_list|,
name|ref
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|cleanup
specifier|static
name|void
name|cleanup
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|)
block|{
name|http_cleanup
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_http_walker
name|struct
name|walker
modifier|*
name|get_http_walker
parameter_list|(
specifier|const
name|char
modifier|*
name|url
parameter_list|,
name|struct
name|remote
modifier|*
name|remote
parameter_list|)
block|{
name|char
modifier|*
name|s
decl_stmt|;
name|struct
name|walker_data
modifier|*
name|data
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|walker_data
argument_list|)
argument_list|)
decl_stmt|;
name|struct
name|walker
modifier|*
name|walker
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|walker
argument_list|)
argument_list|)
decl_stmt|;
name|http_init
argument_list|(
name|remote
argument_list|)
expr_stmt|;
name|data
operator|->
name|alt
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
operator|*
name|data
operator|->
name|alt
argument_list|)
argument_list|)
expr_stmt|;
name|data
operator|->
name|alt
operator|->
name|base
operator|=
name|xmalloc
argument_list|(
name|strlen
argument_list|(
name|url
argument_list|)
operator|+
literal|1
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|data
operator|->
name|alt
operator|->
name|base
argument_list|,
name|url
argument_list|)
expr_stmt|;
for|for
control|(
name|s
operator|=
name|data
operator|->
name|alt
operator|->
name|base
operator|+
name|strlen
argument_list|(
name|data
operator|->
name|alt
operator|->
name|base
argument_list|)
operator|-
literal|1
init|;
operator|*
name|s
operator|==
literal|'/'
condition|;
operator|--
name|s
control|)
operator|*
name|s
operator|=
literal|0
expr_stmt|;
name|data
operator|->
name|alt
operator|->
name|got_indices
operator|=
literal|0
expr_stmt|;
name|data
operator|->
name|alt
operator|->
name|packs
operator|=
name|NULL
expr_stmt|;
name|data
operator|->
name|alt
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
name|data
operator|->
name|got_alternates
operator|=
operator|-
literal|1
expr_stmt|;
name|walker
operator|->
name|corrupt_object_found
operator|=
literal|0
expr_stmt|;
name|walker
operator|->
name|fetch
operator|=
name|fetch
expr_stmt|;
name|walker
operator|->
name|fetch_ref
operator|=
name|fetch_ref
expr_stmt|;
name|walker
operator|->
name|prefetch
operator|=
name|prefetch
expr_stmt|;
name|walker
operator|->
name|cleanup
operator|=
name|cleanup
expr_stmt|;
name|walker
operator|->
name|data
operator|=
name|data
expr_stmt|;
ifdef|#
directive|ifdef
name|USE_CURL_MULTI
name|add_fill_function
argument_list|(
name|walker
argument_list|,
operator|(
name|int
argument_list|(
operator|*
argument_list|)
argument_list|(
name|void
operator|*
argument_list|)
operator|)
name|fill_active_slot
argument_list|)
expr_stmt|;
endif|#
directive|endif
return|return
name|walker
return|;
block|}
end_function
end_unit
