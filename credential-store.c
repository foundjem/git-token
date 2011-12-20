begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"credential.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_decl_stmt
DECL|variable|credential_lock
specifier|static
name|struct
name|lock_file
name|credential_lock
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|parse_credential_file
specifier|static
name|void
name|parse_credential_file
parameter_list|(
specifier|const
name|char
modifier|*
name|fn
parameter_list|,
name|struct
name|credential
modifier|*
name|c
parameter_list|,
name|void
function_decl|(
modifier|*
name|match_cb
function_decl|)
parameter_list|(
name|struct
name|credential
modifier|*
parameter_list|)
parameter_list|,
name|void
function_decl|(
modifier|*
name|other_cb
function_decl|)
parameter_list|(
name|struct
name|strbuf
modifier|*
parameter_list|)
parameter_list|)
block|{
name|FILE
modifier|*
name|fh
decl_stmt|;
name|struct
name|strbuf
name|line
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|credential
name|entry
init|=
name|CREDENTIAL_INIT
decl_stmt|;
name|fh
operator|=
name|fopen
argument_list|(
name|fn
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|fh
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|ENOENT
condition|)
name|die_errno
argument_list|(
literal|"unable to open %s"
argument_list|,
name|fn
argument_list|)
expr_stmt|;
return|return;
block|}
while|while
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|line
argument_list|,
name|fh
argument_list|,
literal|'\n'
argument_list|)
operator|!=
name|EOF
condition|)
block|{
name|credential_from_url
argument_list|(
operator|&
name|entry
argument_list|,
name|line
operator|.
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|entry
operator|.
name|username
operator|&&
name|entry
operator|.
name|password
operator|&&
name|credential_match
argument_list|(
name|c
argument_list|,
operator|&
name|entry
argument_list|)
condition|)
block|{
if|if
condition|(
name|match_cb
condition|)
block|{
name|match_cb
argument_list|(
operator|&
name|entry
argument_list|)
expr_stmt|;
break|break;
block|}
block|}
elseif|else
if|if
condition|(
name|other_cb
condition|)
name|other_cb
argument_list|(
operator|&
name|line
argument_list|)
expr_stmt|;
block|}
name|credential_clear
argument_list|(
operator|&
name|entry
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|line
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|fh
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|print_entry
specifier|static
name|void
name|print_entry
parameter_list|(
name|struct
name|credential
modifier|*
name|c
parameter_list|)
block|{
name|printf
argument_list|(
literal|"username=%s\n"
argument_list|,
name|c
operator|->
name|username
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"password=%s\n"
argument_list|,
name|c
operator|->
name|password
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|print_line
specifier|static
name|void
name|print_line
parameter_list|(
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|)
block|{
name|strbuf_addch
argument_list|(
name|buf
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|credential_lock
operator|.
name|fd
argument_list|,
name|buf
operator|->
name|buf
argument_list|,
name|buf
operator|->
name|len
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|rewrite_credential_file
specifier|static
name|void
name|rewrite_credential_file
parameter_list|(
specifier|const
name|char
modifier|*
name|fn
parameter_list|,
name|struct
name|credential
modifier|*
name|c
parameter_list|,
name|struct
name|strbuf
modifier|*
name|extra
parameter_list|)
block|{
if|if
condition|(
name|hold_lock_file_for_update
argument_list|(
operator|&
name|credential_lock
argument_list|,
name|fn
argument_list|,
literal|0
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"unable to get credential storage lock"
argument_list|)
expr_stmt|;
if|if
condition|(
name|extra
condition|)
name|print_line
argument_list|(
name|extra
argument_list|)
expr_stmt|;
name|parse_credential_file
argument_list|(
name|fn
argument_list|,
name|c
argument_list|,
name|NULL
argument_list|,
name|print_line
argument_list|)
expr_stmt|;
if|if
condition|(
name|commit_lock_file
argument_list|(
operator|&
name|credential_lock
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"unable to commit credential store"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|store_credential
specifier|static
name|void
name|store_credential
parameter_list|(
specifier|const
name|char
modifier|*
name|fn
parameter_list|,
name|struct
name|credential
modifier|*
name|c
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
comment|/* 	 * Sanity check that what we are storing is actually sensible. 	 * In particular, we can't make a URL without a protocol field. 	 * Without either a host or pathname (depending on the scheme), 	 * we have no primary key. And without a username and password, 	 * we are not actually storing a credential. 	 */
if|if
condition|(
operator|!
name|c
operator|->
name|protocol
operator|||
operator|!
operator|(
name|c
operator|->
name|host
operator|||
name|c
operator|->
name|path
operator|)
operator|||
operator|!
name|c
operator|->
name|username
operator|||
operator|!
name|c
operator|->
name|password
condition|)
return|return;
name|strbuf_addf
argument_list|(
operator|&
name|buf
argument_list|,
literal|"%s://"
argument_list|,
name|c
operator|->
name|protocol
argument_list|)
expr_stmt|;
name|strbuf_addstr_urlencode
argument_list|(
operator|&
name|buf
argument_list|,
name|c
operator|->
name|username
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
operator|&
name|buf
argument_list|,
literal|':'
argument_list|)
expr_stmt|;
name|strbuf_addstr_urlencode
argument_list|(
operator|&
name|buf
argument_list|,
name|c
operator|->
name|password
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
operator|&
name|buf
argument_list|,
literal|'@'
argument_list|)
expr_stmt|;
if|if
condition|(
name|c
operator|->
name|host
condition|)
name|strbuf_addstr_urlencode
argument_list|(
operator|&
name|buf
argument_list|,
name|c
operator|->
name|host
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|c
operator|->
name|path
condition|)
block|{
name|strbuf_addch
argument_list|(
operator|&
name|buf
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
name|strbuf_addstr_urlencode
argument_list|(
operator|&
name|buf
argument_list|,
name|c
operator|->
name|path
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
name|rewrite_credential_file
argument_list|(
name|fn
argument_list|,
name|c
argument_list|,
operator|&
name|buf
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|remove_credential
specifier|static
name|void
name|remove_credential
parameter_list|(
specifier|const
name|char
modifier|*
name|fn
parameter_list|,
name|struct
name|credential
modifier|*
name|c
parameter_list|)
block|{
comment|/* 	 * Sanity check that we actually have something to match 	 * against. The input we get is a restrictive pattern, 	 * so technically a blank credential means "erase everything". 	 * But it is too easy to accidentally send this, since it is equivalent 	 * to empty input. So explicitly disallow it, and require that the 	 * pattern have some actual content to match. 	 */
if|if
condition|(
name|c
operator|->
name|protocol
operator|||
name|c
operator|->
name|host
operator|||
name|c
operator|->
name|path
operator|||
name|c
operator|->
name|username
condition|)
name|rewrite_credential_file
argument_list|(
name|fn
argument_list|,
name|c
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|lookup_credential
specifier|static
name|int
name|lookup_credential
parameter_list|(
specifier|const
name|char
modifier|*
name|fn
parameter_list|,
name|struct
name|credential
modifier|*
name|c
parameter_list|)
block|{
name|parse_credential_file
argument_list|(
name|fn
argument_list|,
name|c
argument_list|,
name|print_entry
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
return|return
name|c
operator|->
name|username
operator|&&
name|c
operator|->
name|password
return|;
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
specifier|const
name|usage
index|[]
init|=
block|{
literal|"git credential-store [options]<action>"
block|,
name|NULL
block|}
decl_stmt|;
specifier|const
name|char
modifier|*
name|op
decl_stmt|;
name|struct
name|credential
name|c
init|=
name|CREDENTIAL_INIT
decl_stmt|;
name|char
modifier|*
name|file
init|=
name|NULL
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"file"
argument_list|,
operator|&
name|file
argument_list|,
literal|"path"
argument_list|,
literal|"fetch and store credentials in<path>"
argument_list|)
block|,
name|OPT_END
argument_list|()
block|}
decl_stmt|;
name|umask
argument_list|(
literal|077
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
if|if
condition|(
name|argc
operator|!=
literal|1
condition|)
name|usage_with_options
argument_list|(
name|usage
argument_list|,
name|options
argument_list|)
expr_stmt|;
name|op
operator|=
name|argv
index|[
literal|0
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|file
condition|)
name|file
operator|=
name|expand_user_path
argument_list|(
literal|"~/.git-credentials"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|file
condition|)
name|die
argument_list|(
literal|"unable to set up default path; use --file"
argument_list|)
expr_stmt|;
if|if
condition|(
name|credential_read
argument_list|(
operator|&
name|c
argument_list|,
name|stdin
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to read credential"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|op
argument_list|,
literal|"get"
argument_list|)
condition|)
name|lookup_credential
argument_list|(
name|file
argument_list|,
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
name|op
argument_list|,
literal|"erase"
argument_list|)
condition|)
name|remove_credential
argument_list|(
name|file
argument_list|,
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
name|op
argument_list|,
literal|"store"
argument_list|)
condition|)
name|store_credential
argument_list|(
name|file
argument_list|,
operator|&
name|c
argument_list|)
expr_stmt|;
else|else
empty_stmt|;
comment|/* Ignore unknown operation. */
return|return
literal|0
return|;
block|}
end_function
end_unit
