begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_include
include|#
directive|include
file|"object.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_decl_stmt
DECL|variable|content_type
specifier|static
specifier|const
name|char
name|content_type
index|[]
init|=
literal|"Content-Type"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|content_length
specifier|static
specifier|const
name|char
name|content_length
index|[]
init|=
literal|"Content-Length"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|last_modified
specifier|static
specifier|const
name|char
name|last_modified
index|[]
init|=
literal|"Last-Modified"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|format_write
specifier|static
name|void
name|format_write
parameter_list|(
name|int
name|fd
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
modifier|...
parameter_list|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|1024
index|]
decl_stmt|;
name|va_list
name|args
decl_stmt|;
name|unsigned
name|n
decl_stmt|;
name|va_start
argument_list|(
name|args
argument_list|,
name|fmt
argument_list|)
expr_stmt|;
name|n
operator|=
name|vsnprintf
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|fmt
argument_list|,
name|args
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|args
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|>=
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
condition|)
name|die
argument_list|(
literal|"protocol error: impossibly long line"
argument_list|)
expr_stmt|;
name|safe_write
argument_list|(
name|fd
argument_list|,
name|buffer
argument_list|,
name|n
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|http_status
specifier|static
name|void
name|http_status
parameter_list|(
name|unsigned
name|code
parameter_list|,
specifier|const
name|char
modifier|*
name|msg
parameter_list|)
block|{
name|format_write
argument_list|(
literal|1
argument_list|,
literal|"Status: %u %s\r\n"
argument_list|,
name|code
argument_list|,
name|msg
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|hdr_str
specifier|static
name|void
name|hdr_str
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
name|format_write
argument_list|(
literal|1
argument_list|,
literal|"%s: %s\r\n"
argument_list|,
name|name
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|hdr_int
specifier|static
name|void
name|hdr_int
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|size_t
name|value
parameter_list|)
block|{
name|format_write
argument_list|(
literal|1
argument_list|,
literal|"%s: %"
name|PRIuMAX
literal|"\r\n"
argument_list|,
name|name
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|hdr_date
specifier|static
name|void
name|hdr_date
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|unsigned
name|long
name|when
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|value
init|=
name|show_date
argument_list|(
name|when
argument_list|,
literal|0
argument_list|,
name|DATE_RFC2822
argument_list|)
decl_stmt|;
name|hdr_str
argument_list|(
name|name
argument_list|,
name|value
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|hdr_nocache
specifier|static
name|void
name|hdr_nocache
parameter_list|(
name|void
parameter_list|)
block|{
name|hdr_str
argument_list|(
literal|"Expires"
argument_list|,
literal|"Fri, 01 Jan 1980 00:00:00 GMT"
argument_list|)
expr_stmt|;
name|hdr_str
argument_list|(
literal|"Pragma"
argument_list|,
literal|"no-cache"
argument_list|)
expr_stmt|;
name|hdr_str
argument_list|(
literal|"Cache-Control"
argument_list|,
literal|"no-cache, max-age=0, must-revalidate"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|hdr_cache_forever
specifier|static
name|void
name|hdr_cache_forever
parameter_list|(
name|void
parameter_list|)
block|{
name|unsigned
name|long
name|now
init|=
name|time
argument_list|(
name|NULL
argument_list|)
decl_stmt|;
name|hdr_date
argument_list|(
literal|"Date"
argument_list|,
name|now
argument_list|)
expr_stmt|;
name|hdr_date
argument_list|(
literal|"Expires"
argument_list|,
name|now
operator|+
literal|31536000
argument_list|)
expr_stmt|;
name|hdr_str
argument_list|(
literal|"Cache-Control"
argument_list|,
literal|"public, max-age=31536000"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|end_headers
specifier|static
name|void
name|end_headers
parameter_list|(
name|void
parameter_list|)
block|{
name|safe_write
argument_list|(
literal|1
argument_list|,
literal|"\r\n"
argument_list|,
literal|2
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|not_found
specifier|static
name|NORETURN
name|void
name|not_found
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|,
modifier|...
parameter_list|)
block|{
name|va_list
name|params
decl_stmt|;
name|http_status
argument_list|(
literal|404
argument_list|,
literal|"Not Found"
argument_list|)
expr_stmt|;
name|hdr_nocache
argument_list|()
expr_stmt|;
name|end_headers
argument_list|()
expr_stmt|;
name|va_start
argument_list|(
name|params
argument_list|,
name|err
argument_list|)
expr_stmt|;
if|if
condition|(
name|err
operator|&&
operator|*
name|err
condition|)
name|vfprintf
argument_list|(
name|stderr
argument_list|,
name|err
argument_list|,
name|params
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|params
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|send_strbuf
specifier|static
name|void
name|send_strbuf
parameter_list|(
specifier|const
name|char
modifier|*
name|type
parameter_list|,
name|struct
name|strbuf
modifier|*
name|buf
parameter_list|)
block|{
name|hdr_int
argument_list|(
name|content_length
argument_list|,
name|buf
operator|->
name|len
argument_list|)
expr_stmt|;
name|hdr_str
argument_list|(
name|content_type
argument_list|,
name|type
argument_list|)
expr_stmt|;
name|end_headers
argument_list|()
expr_stmt|;
name|safe_write
argument_list|(
literal|1
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
DECL|function|send_file
specifier|static
name|void
name|send_file
parameter_list|(
specifier|const
name|char
modifier|*
name|the_type
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|p
init|=
name|git_path
argument_list|(
literal|"%s"
argument_list|,
name|name
argument_list|)
decl_stmt|;
name|size_t
name|buf_alloc
init|=
literal|8192
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|xmalloc
argument_list|(
name|buf_alloc
argument_list|)
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|struct
name|stat
name|sb
decl_stmt|;
name|size_t
name|size
decl_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|p
argument_list|,
name|O_RDONLY
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|not_found
argument_list|(
literal|"Cannot open '%s': %s"
argument_list|,
name|p
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|fstat
argument_list|(
name|fd
argument_list|,
operator|&
name|sb
argument_list|)
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"Cannot stat '%s'"
argument_list|,
name|p
argument_list|)
expr_stmt|;
name|size
operator|=
name|xsize_t
argument_list|(
name|sb
operator|.
name|st_size
argument_list|)
expr_stmt|;
name|hdr_int
argument_list|(
name|content_length
argument_list|,
name|size
argument_list|)
expr_stmt|;
name|hdr_str
argument_list|(
name|content_type
argument_list|,
name|the_type
argument_list|)
expr_stmt|;
name|hdr_date
argument_list|(
name|last_modified
argument_list|,
name|sb
operator|.
name|st_mtime
argument_list|)
expr_stmt|;
name|end_headers
argument_list|()
expr_stmt|;
while|while
condition|(
name|size
condition|)
block|{
name|ssize_t
name|n
init|=
name|xread
argument_list|(
name|fd
argument_list|,
name|buf
argument_list|,
name|buf_alloc
argument_list|)
decl_stmt|;
if|if
condition|(
name|n
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"Cannot read '%s'"
argument_list|,
name|p
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|n
condition|)
break|break;
name|safe_write
argument_list|(
literal|1
argument_list|,
name|buf
argument_list|,
name|n
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_text_file
specifier|static
name|void
name|get_text_file
parameter_list|(
name|char
modifier|*
name|name
parameter_list|)
block|{
name|hdr_nocache
argument_list|()
expr_stmt|;
name|send_file
argument_list|(
literal|"text/plain"
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_loose_object
specifier|static
name|void
name|get_loose_object
parameter_list|(
name|char
modifier|*
name|name
parameter_list|)
block|{
name|hdr_cache_forever
argument_list|()
expr_stmt|;
name|send_file
argument_list|(
literal|"application/x-git-loose-object"
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_pack_file
specifier|static
name|void
name|get_pack_file
parameter_list|(
name|char
modifier|*
name|name
parameter_list|)
block|{
name|hdr_cache_forever
argument_list|()
expr_stmt|;
name|send_file
argument_list|(
literal|"application/x-git-packed-objects"
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|get_idx_file
specifier|static
name|void
name|get_idx_file
parameter_list|(
name|char
modifier|*
name|name
parameter_list|)
block|{
name|hdr_cache_forever
argument_list|()
expr_stmt|;
name|send_file
argument_list|(
literal|"application/x-git-packed-objects-toc"
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_text_ref
specifier|static
name|int
name|show_text_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|flag
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|struct
name|strbuf
modifier|*
name|buf
init|=
name|cb_data
decl_stmt|;
name|struct
name|object
modifier|*
name|o
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|o
condition|)
return|return
literal|0
return|;
name|strbuf_addf
argument_list|(
name|buf
argument_list|,
literal|"%s\t%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|o
operator|->
name|type
operator|==
name|OBJ_TAG
condition|)
block|{
name|o
operator|=
name|deref_tag
argument_list|(
name|o
argument_list|,
name|name
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|o
condition|)
return|return
literal|0
return|;
name|strbuf_addf
argument_list|(
name|buf
argument_list|,
literal|"%s\t%s^{}\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|o
operator|->
name|sha1
argument_list|)
argument_list|,
name|name
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|get_info_refs
specifier|static
name|void
name|get_info_refs
parameter_list|(
name|char
modifier|*
name|arg
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|for_each_ref
argument_list|(
name|show_text_ref
argument_list|,
operator|&
name|buf
argument_list|)
expr_stmt|;
name|hdr_nocache
argument_list|()
expr_stmt|;
name|send_strbuf
argument_list|(
literal|"text/plain"
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
DECL|function|get_info_packs
specifier|static
name|void
name|get_info_packs
parameter_list|(
name|char
modifier|*
name|arg
parameter_list|)
block|{
name|size_t
name|objdirlen
init|=
name|strlen
argument_list|(
name|get_object_directory
argument_list|()
argument_list|)
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|packed_git
modifier|*
name|p
decl_stmt|;
name|size_t
name|cnt
init|=
literal|0
decl_stmt|;
name|prepare_packed_git
argument_list|()
expr_stmt|;
for|for
control|(
name|p
operator|=
name|packed_git
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
if|if
condition|(
name|p
operator|->
name|pack_local
condition|)
name|cnt
operator|++
expr_stmt|;
block|}
name|strbuf_grow
argument_list|(
operator|&
name|buf
argument_list|,
name|cnt
operator|*
literal|53
operator|+
literal|2
argument_list|)
expr_stmt|;
for|for
control|(
name|p
operator|=
name|packed_git
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
if|if
condition|(
name|p
operator|->
name|pack_local
condition|)
name|strbuf_addf
argument_list|(
operator|&
name|buf
argument_list|,
literal|"P %s\n"
argument_list|,
name|p
operator|->
name|pack_name
operator|+
name|objdirlen
operator|+
literal|6
argument_list|)
expr_stmt|;
block|}
name|strbuf_addch
argument_list|(
operator|&
name|buf
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
name|hdr_nocache
argument_list|()
expr_stmt|;
name|send_strbuf
argument_list|(
literal|"text/plain; charset=utf-8"
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
DECL|function|die_webcgi
specifier|static
name|NORETURN
name|void
name|die_webcgi
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|,
name|va_list
name|params
parameter_list|)
block|{
name|char
name|buffer
index|[
literal|1000
index|]
decl_stmt|;
name|http_status
argument_list|(
literal|500
argument_list|,
literal|"Internal Server Error"
argument_list|)
expr_stmt|;
name|hdr_nocache
argument_list|()
expr_stmt|;
name|end_headers
argument_list|()
expr_stmt|;
name|vsnprintf
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
name|err
argument_list|,
name|params
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"fatal: %s\n"
argument_list|,
name|buffer
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
end_function
begin_struct
DECL|struct|service_cmd
specifier|static
struct|struct
name|service_cmd
block|{
DECL|member|method
specifier|const
name|char
modifier|*
name|method
decl_stmt|;
DECL|member|pattern
specifier|const
name|char
modifier|*
name|pattern
decl_stmt|;
DECL|member|imp
name|void
function_decl|(
modifier|*
name|imp
function_decl|)
parameter_list|(
name|char
modifier|*
parameter_list|)
function_decl|;
block|}
DECL|variable|services
name|services
index|[]
init|=
block|{
block|{
literal|"GET"
block|,
literal|"/HEAD$"
block|,
name|get_text_file
block|}
block|,
block|{
literal|"GET"
block|,
literal|"/info/refs$"
block|,
name|get_info_refs
block|}
block|,
block|{
literal|"GET"
block|,
literal|"/objects/info/alternates$"
block|,
name|get_text_file
block|}
block|,
block|{
literal|"GET"
block|,
literal|"/objects/info/http-alternates$"
block|,
name|get_text_file
block|}
block|,
block|{
literal|"GET"
block|,
literal|"/objects/info/packs$"
block|,
name|get_info_packs
block|}
block|,
block|{
literal|"GET"
block|,
literal|"/objects/[0-9a-f]{2}/[0-9a-f]{38}$"
block|,
name|get_loose_object
block|}
block|,
block|{
literal|"GET"
block|,
literal|"/objects/pack/pack-[0-9a-f]{40}\\.pack$"
block|,
name|get_pack_file
block|}
block|,
block|{
literal|"GET"
block|,
literal|"/objects/pack/pack-[0-9a-f]{40}\\.idx$"
block|,
name|get_idx_file
block|}
block|}
struct|;
end_struct
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|char
modifier|*
name|method
init|=
name|getenv
argument_list|(
literal|"REQUEST_METHOD"
argument_list|)
decl_stmt|;
name|char
modifier|*
name|dir
init|=
name|getenv
argument_list|(
literal|"PATH_TRANSLATED"
argument_list|)
decl_stmt|;
name|struct
name|service_cmd
modifier|*
name|cmd
init|=
name|NULL
decl_stmt|;
name|char
modifier|*
name|cmd_arg
init|=
name|NULL
decl_stmt|;
name|int
name|i
decl_stmt|;
name|git_extract_argv0_path
argument_list|(
name|argv
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|set_die_routine
argument_list|(
name|die_webcgi
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|method
condition|)
name|die
argument_list|(
literal|"No REQUEST_METHOD from server"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|method
argument_list|,
literal|"HEAD"
argument_list|)
condition|)
name|method
operator|=
literal|"GET"
expr_stmt|;
if|if
condition|(
operator|!
name|dir
condition|)
name|die
argument_list|(
literal|"No PATH_TRANSLATED from server"
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|ARRAY_SIZE
argument_list|(
name|services
argument_list|)
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|service_cmd
modifier|*
name|c
init|=
operator|&
name|services
index|[
name|i
index|]
decl_stmt|;
name|regex_t
name|re
decl_stmt|;
name|regmatch_t
name|out
index|[
literal|1
index|]
decl_stmt|;
if|if
condition|(
name|regcomp
argument_list|(
operator|&
name|re
argument_list|,
name|c
operator|->
name|pattern
argument_list|,
name|REG_EXTENDED
argument_list|)
condition|)
name|die
argument_list|(
literal|"Bogus regex in service table: %s"
argument_list|,
name|c
operator|->
name|pattern
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|regexec
argument_list|(
operator|&
name|re
argument_list|,
name|dir
argument_list|,
literal|1
argument_list|,
name|out
argument_list|,
literal|0
argument_list|)
condition|)
block|{
name|size_t
name|n
init|=
name|out
index|[
literal|0
index|]
operator|.
name|rm_eo
operator|-
name|out
index|[
literal|0
index|]
operator|.
name|rm_so
decl_stmt|;
if|if
condition|(
name|strcmp
argument_list|(
name|method
argument_list|,
name|c
operator|->
name|method
argument_list|)
condition|)
block|{
specifier|const
name|char
modifier|*
name|proto
init|=
name|getenv
argument_list|(
literal|"SERVER_PROTOCOL"
argument_list|)
decl_stmt|;
if|if
condition|(
name|proto
operator|&&
operator|!
name|strcmp
argument_list|(
name|proto
argument_list|,
literal|"HTTP/1.1"
argument_list|)
condition|)
name|http_status
argument_list|(
literal|405
argument_list|,
literal|"Method Not Allowed"
argument_list|)
expr_stmt|;
else|else
name|http_status
argument_list|(
literal|400
argument_list|,
literal|"Bad Request"
argument_list|)
expr_stmt|;
name|hdr_nocache
argument_list|()
expr_stmt|;
name|end_headers
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
name|cmd
operator|=
name|c
expr_stmt|;
name|cmd_arg
operator|=
name|xmalloc
argument_list|(
name|n
argument_list|)
expr_stmt|;
name|strncpy
argument_list|(
name|cmd_arg
argument_list|,
name|dir
operator|+
name|out
index|[
literal|0
index|]
operator|.
name|rm_so
operator|+
literal|1
argument_list|,
name|n
argument_list|)
expr_stmt|;
name|cmd_arg
index|[
name|n
index|]
operator|=
literal|'\0'
expr_stmt|;
name|dir
index|[
name|out
index|[
literal|0
index|]
operator|.
name|rm_so
index|]
operator|=
literal|0
expr_stmt|;
break|break;
block|}
name|regfree
argument_list|(
operator|&
name|re
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|cmd
condition|)
name|not_found
argument_list|(
literal|"Request not supported: '%s'"
argument_list|,
name|dir
argument_list|)
expr_stmt|;
name|setup_path
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|enter_repo
argument_list|(
name|dir
argument_list|,
literal|0
argument_list|)
condition|)
name|not_found
argument_list|(
literal|"Not a git repository: '%s'"
argument_list|,
name|dir
argument_list|)
expr_stmt|;
name|cmd
operator|->
name|imp
argument_list|(
name|cmd_arg
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
