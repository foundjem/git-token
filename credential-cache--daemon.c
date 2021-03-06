begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"tempfile.h"
end_include
begin_include
include|#
directive|include
file|"credential.h"
end_include
begin_include
include|#
directive|include
file|"unix-socket.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|socket_file
specifier|static
name|struct
name|tempfile
name|socket_file
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|credential_cache_entry
struct|struct
name|credential_cache_entry
block|{
DECL|member|item
name|struct
name|credential
name|item
decl_stmt|;
DECL|member|expiration
name|unsigned
name|long
name|expiration
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|entries
specifier|static
name|struct
name|credential_cache_entry
modifier|*
name|entries
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|entries_nr
specifier|static
name|int
name|entries_nr
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|entries_alloc
specifier|static
name|int
name|entries_alloc
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cache_credential
specifier|static
name|void
name|cache_credential
parameter_list|(
name|struct
name|credential
modifier|*
name|c
parameter_list|,
name|int
name|timeout
parameter_list|)
block|{
name|struct
name|credential_cache_entry
modifier|*
name|e
decl_stmt|;
name|ALLOC_GROW
argument_list|(
name|entries
argument_list|,
name|entries_nr
operator|+
literal|1
argument_list|,
name|entries_alloc
argument_list|)
expr_stmt|;
name|e
operator|=
operator|&
name|entries
index|[
name|entries_nr
operator|++
index|]
expr_stmt|;
comment|/* take ownership of pointers */
name|memcpy
argument_list|(
operator|&
name|e
operator|->
name|item
argument_list|,
name|c
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|c
argument_list|)
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|c
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|c
argument_list|)
argument_list|)
expr_stmt|;
name|e
operator|->
name|expiration
operator|=
name|time
argument_list|(
name|NULL
argument_list|)
operator|+
name|timeout
expr_stmt|;
block|}
end_function
begin_function
DECL|function|lookup_credential
specifier|static
name|struct
name|credential_cache_entry
modifier|*
name|lookup_credential
parameter_list|(
specifier|const
name|struct
name|credential
modifier|*
name|c
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|entries_nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|credential
modifier|*
name|e
init|=
operator|&
name|entries
index|[
name|i
index|]
operator|.
name|item
decl_stmt|;
if|if
condition|(
name|credential_match
argument_list|(
name|c
argument_list|,
name|e
argument_list|)
condition|)
return|return
operator|&
name|entries
index|[
name|i
index|]
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_function
DECL|function|remove_credential
specifier|static
name|void
name|remove_credential
parameter_list|(
specifier|const
name|struct
name|credential
modifier|*
name|c
parameter_list|)
block|{
name|struct
name|credential_cache_entry
modifier|*
name|e
decl_stmt|;
name|e
operator|=
name|lookup_credential
argument_list|(
name|c
argument_list|)
expr_stmt|;
if|if
condition|(
name|e
condition|)
name|e
operator|->
name|expiration
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|check_expirations
specifier|static
name|int
name|check_expirations
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|unsigned
name|long
name|wait_for_entry_until
decl_stmt|;
name|int
name|i
init|=
literal|0
decl_stmt|;
name|unsigned
name|long
name|now
init|=
name|time
argument_list|(
name|NULL
argument_list|)
decl_stmt|;
name|unsigned
name|long
name|next
init|=
operator|(
name|unsigned
name|long
operator|)
operator|-
literal|1
decl_stmt|;
comment|/* 	 * Initially give the client 30 seconds to actually contact us 	 * and store a credential before we decide there's no point in 	 * keeping the daemon around. 	 */
if|if
condition|(
operator|!
name|wait_for_entry_until
condition|)
name|wait_for_entry_until
operator|=
name|now
operator|+
literal|30
expr_stmt|;
while|while
condition|(
name|i
operator|<
name|entries_nr
condition|)
block|{
if|if
condition|(
name|entries
index|[
name|i
index|]
operator|.
name|expiration
operator|<=
name|now
condition|)
block|{
name|entries_nr
operator|--
expr_stmt|;
name|credential_clear
argument_list|(
operator|&
name|entries
index|[
name|i
index|]
operator|.
name|item
argument_list|)
expr_stmt|;
if|if
condition|(
name|i
operator|!=
name|entries_nr
condition|)
name|memcpy
argument_list|(
operator|&
name|entries
index|[
name|i
index|]
argument_list|,
operator|&
name|entries
index|[
name|entries_nr
index|]
argument_list|,
sizeof|sizeof
argument_list|(
operator|*
name|entries
argument_list|)
argument_list|)
expr_stmt|;
comment|/* 			 * Stick around 30 seconds in case a new credential 			 * shows up (e.g., because we just removed a failed 			 * one, and we will soon get the correct one). 			 */
name|wait_for_entry_until
operator|=
name|now
operator|+
literal|30
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|entries
index|[
name|i
index|]
operator|.
name|expiration
operator|<
name|next
condition|)
name|next
operator|=
name|entries
index|[
name|i
index|]
operator|.
name|expiration
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
block|}
if|if
condition|(
operator|!
name|entries_nr
condition|)
block|{
if|if
condition|(
name|wait_for_entry_until
operator|<=
name|now
condition|)
return|return
literal|0
return|;
name|next
operator|=
name|wait_for_entry_until
expr_stmt|;
block|}
return|return
name|next
operator|-
name|now
return|;
block|}
end_function
begin_function
DECL|function|read_request
specifier|static
name|int
name|read_request
parameter_list|(
name|FILE
modifier|*
name|fh
parameter_list|,
name|struct
name|credential
modifier|*
name|c
parameter_list|,
name|struct
name|strbuf
modifier|*
name|action
parameter_list|,
name|int
modifier|*
name|timeout
parameter_list|)
block|{
specifier|static
name|struct
name|strbuf
name|item
init|=
name|STRBUF_INIT
decl_stmt|;
specifier|const
name|char
modifier|*
name|p
decl_stmt|;
name|strbuf_getline_lf
argument_list|(
operator|&
name|item
argument_list|,
name|fh
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|skip_prefix
argument_list|(
name|item
operator|.
name|buf
argument_list|,
literal|"action="
argument_list|,
operator|&
name|p
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"client sent bogus action line: %s"
argument_list|,
name|item
operator|.
name|buf
argument_list|)
return|;
name|strbuf_addstr
argument_list|(
name|action
argument_list|,
name|p
argument_list|)
expr_stmt|;
name|strbuf_getline_lf
argument_list|(
operator|&
name|item
argument_list|,
name|fh
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|skip_prefix
argument_list|(
name|item
operator|.
name|buf
argument_list|,
literal|"timeout="
argument_list|,
operator|&
name|p
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"client sent bogus timeout line: %s"
argument_list|,
name|item
operator|.
name|buf
argument_list|)
return|;
operator|*
name|timeout
operator|=
name|atoi
argument_list|(
name|p
argument_list|)
expr_stmt|;
if|if
condition|(
name|credential_read
argument_list|(
name|c
argument_list|,
name|fh
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|serve_one_client
specifier|static
name|void
name|serve_one_client
parameter_list|(
name|FILE
modifier|*
name|in
parameter_list|,
name|FILE
modifier|*
name|out
parameter_list|)
block|{
name|struct
name|credential
name|c
init|=
name|CREDENTIAL_INIT
decl_stmt|;
name|struct
name|strbuf
name|action
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|timeout
init|=
operator|-
literal|1
decl_stmt|;
if|if
condition|(
name|read_request
argument_list|(
name|in
argument_list|,
operator|&
name|c
argument_list|,
operator|&
name|action
argument_list|,
operator|&
name|timeout
argument_list|)
operator|<
literal|0
condition|)
comment|/* ignore error */
empty_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|action
operator|.
name|buf
argument_list|,
literal|"get"
argument_list|)
condition|)
block|{
name|struct
name|credential_cache_entry
modifier|*
name|e
init|=
name|lookup_credential
argument_list|(
operator|&
name|c
argument_list|)
decl_stmt|;
if|if
condition|(
name|e
condition|)
block|{
name|fprintf
argument_list|(
name|out
argument_list|,
literal|"username=%s\n"
argument_list|,
name|e
operator|->
name|item
operator|.
name|username
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|out
argument_list|,
literal|"password=%s\n"
argument_list|,
name|e
operator|->
name|item
operator|.
name|password
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|action
operator|.
name|buf
argument_list|,
literal|"exit"
argument_list|)
condition|)
block|{
comment|/* 		 * It's important that we clean up our socket first, and then 		 * signal the client only once we have finished the cleanup. 		 * Calling exit() directly does this, because we clean up in 		 * our atexit() handler, and then signal the client when our 		 * process actually ends, which closes the socket and gives 		 * them EOF. 		 */
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|action
operator|.
name|buf
argument_list|,
literal|"erase"
argument_list|)
condition|)
name|remove_credential
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|action
operator|.
name|buf
argument_list|,
literal|"store"
argument_list|)
condition|)
block|{
if|if
condition|(
name|timeout
operator|<
literal|0
condition|)
name|warning
argument_list|(
literal|"cache client didn't specify a timeout"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|c
operator|.
name|username
operator|||
operator|!
name|c
operator|.
name|password
condition|)
name|warning
argument_list|(
literal|"cache client gave us a partial credential"
argument_list|)
expr_stmt|;
else|else
block|{
name|remove_credential
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
name|cache_credential
argument_list|(
operator|&
name|c
argument_list|,
name|timeout
argument_list|)
expr_stmt|;
block|}
block|}
else|else
name|warning
argument_list|(
literal|"cache client sent unknown action: %s"
argument_list|,
name|action
operator|.
name|buf
argument_list|)
expr_stmt|;
name|credential_clear
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|action
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|serve_cache_loop
specifier|static
name|int
name|serve_cache_loop
parameter_list|(
name|int
name|fd
parameter_list|)
block|{
name|struct
name|pollfd
name|pfd
decl_stmt|;
name|unsigned
name|long
name|wakeup
decl_stmt|;
name|wakeup
operator|=
name|check_expirations
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|wakeup
condition|)
return|return
literal|0
return|;
name|pfd
operator|.
name|fd
operator|=
name|fd
expr_stmt|;
name|pfd
operator|.
name|events
operator|=
name|POLLIN
expr_stmt|;
if|if
condition|(
name|poll
argument_list|(
operator|&
name|pfd
argument_list|,
literal|1
argument_list|,
literal|1000
operator|*
name|wakeup
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|EINTR
condition|)
name|die_errno
argument_list|(
literal|"poll failed"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
if|if
condition|(
name|pfd
operator|.
name|revents
operator|&
name|POLLIN
condition|)
block|{
name|int
name|client
decl_stmt|,
name|client2
decl_stmt|;
name|FILE
modifier|*
name|in
decl_stmt|,
modifier|*
name|out
decl_stmt|;
name|client
operator|=
name|accept
argument_list|(
name|fd
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|client
operator|<
literal|0
condition|)
block|{
name|warning_errno
argument_list|(
literal|"accept failed"
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|client2
operator|=
name|dup
argument_list|(
name|client
argument_list|)
expr_stmt|;
if|if
condition|(
name|client2
operator|<
literal|0
condition|)
block|{
name|warning_errno
argument_list|(
literal|"dup failed"
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|client
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
name|in
operator|=
name|xfdopen
argument_list|(
name|client
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
name|out
operator|=
name|xfdopen
argument_list|(
name|client2
argument_list|,
literal|"w"
argument_list|)
expr_stmt|;
name|serve_one_client
argument_list|(
name|in
argument_list|,
name|out
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|in
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|out
argument_list|)
expr_stmt|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|serve_cache
specifier|static
name|void
name|serve_cache
parameter_list|(
specifier|const
name|char
modifier|*
name|socket_path
parameter_list|,
name|int
name|debug
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|fd
operator|=
name|unix_stream_listen
argument_list|(
name|socket_path
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"unable to bind to '%s'"
argument_list|,
name|socket_path
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"ok\n"
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|debug
condition|)
block|{
if|if
condition|(
operator|!
name|freopen
argument_list|(
literal|"/dev/null"
argument_list|,
literal|"w"
argument_list|,
name|stderr
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"unable to point stderr to /dev/null"
argument_list|)
expr_stmt|;
block|}
while|while
condition|(
name|serve_cache_loop
argument_list|(
name|fd
argument_list|)
condition|)
empty_stmt|;
comment|/* nothing */
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|permissions_advice
specifier|static
specifier|const
name|char
name|permissions_advice
index|[]
init|=
literal|"The permissions on your socket directory are too loose; other\n"
literal|"users may be able to read your cached credentials. Consider running:\n"
literal|"\n"
literal|"	chmod 0700 %s"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|init_socket_directory
specifier|static
name|void
name|init_socket_directory
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
name|char
modifier|*
name|path_copy
init|=
name|xstrdup
argument_list|(
name|path
argument_list|)
decl_stmt|;
name|char
modifier|*
name|dir
init|=
name|dirname
argument_list|(
name|path_copy
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|stat
argument_list|(
name|dir
argument_list|,
operator|&
name|st
argument_list|)
condition|)
block|{
if|if
condition|(
name|st
operator|.
name|st_mode
operator|&
literal|077
condition|)
name|die
argument_list|(
name|permissions_advice
argument_list|,
name|dir
argument_list|)
expr_stmt|;
block|}
else|else
block|{
comment|/* 		 * We must be sure to create the directory with the correct mode, 		 * not just chmod it after the fact; otherwise, there is a race 		 * condition in which somebody can chdir to it, sleep, then try to open 		 * our protected socket. 		 */
if|if
condition|(
name|safe_create_leading_directories_const
argument_list|(
name|dir
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"unable to create directories for '%s'"
argument_list|,
name|dir
argument_list|)
expr_stmt|;
if|if
condition|(
name|mkdir
argument_list|(
name|dir
argument_list|,
literal|0700
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"unable to mkdir '%s'"
argument_list|,
name|dir
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|chdir
argument_list|(
name|dir
argument_list|)
condition|)
comment|/* 		 * We don't actually care what our cwd is; we chdir here just to 		 * be a friendly daemon and avoid tying up our original cwd. 		 * If this fails, it's OK to just continue without that benefit. 		 */
empty_stmt|;
name|free
argument_list|(
name|path_copy
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|socket_path
decl_stmt|;
name|int
name|ignore_sighup
init|=
literal|0
decl_stmt|;
specifier|static
specifier|const
name|char
modifier|*
name|usage
index|[]
init|=
block|{
literal|"git-credential-cache--daemon [opts]<socket_path>"
block|,
name|NULL
block|}
decl_stmt|;
name|int
name|debug
init|=
literal|0
decl_stmt|;
specifier|const
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"debug"
argument_list|,
operator|&
name|debug
argument_list|,
name|N_
argument_list|(
literal|"print debugging messages to stderr"
argument_list|)
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|git_config_get_bool
argument_list|(
literal|"credentialcache.ignoresighup"
argument_list|,
operator|&
name|ignore_sighup
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|NULL
argument_list|,
name|options
argument_list|,
name|usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|socket_path
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|socket_path
condition|)
name|usage_with_options
argument_list|(
name|usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|is_absolute_path
argument_list|(
name|socket_path
argument_list|)
condition|)
name|die
argument_list|(
literal|"socket directory must be an absolute path"
argument_list|)
expr_stmt|;
name|init_socket_directory
argument_list|(
name|socket_path
argument_list|)
expr_stmt|;
name|register_tempfile
argument_list|(
operator|&
name|socket_file
argument_list|,
name|socket_path
argument_list|)
expr_stmt|;
if|if
condition|(
name|ignore_sighup
condition|)
name|signal
argument_list|(
name|SIGHUP
argument_list|,
name|SIG_IGN
argument_list|)
expr_stmt|;
name|serve_cache
argument_list|(
name|socket_path
argument_list|,
name|debug
argument_list|)
expr_stmt|;
name|delete_tempfile
argument_list|(
operator|&
name|socket_file
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
