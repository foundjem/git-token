begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"rsh.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|<string.h>
end_include
begin_decl_stmt
DECL|variable|local_version
specifier|static
name|unsigned
name|char
name|local_version
init|=
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|remote_version
specifier|static
name|unsigned
name|char
name|remote_version
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|verbose
specifier|static
name|int
name|verbose
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|serve_object
specifier|static
name|int
name|serve_object
parameter_list|(
name|int
name|fd_in
parameter_list|,
name|int
name|fd_out
parameter_list|)
block|{
name|ssize_t
name|size
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|signed
name|char
name|remote
decl_stmt|;
name|int
name|posn
init|=
literal|0
decl_stmt|;
do|do
block|{
name|size
operator|=
name|read
argument_list|(
name|fd_in
argument_list|,
name|sha1
operator|+
name|posn
argument_list|,
literal|20
operator|-
name|posn
argument_list|)
expr_stmt|;
if|if
condition|(
name|size
operator|<
literal|0
condition|)
block|{
name|perror
argument_list|(
literal|"git-ssh-push: read "
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
operator|!
name|size
condition|)
return|return
operator|-
literal|1
return|;
name|posn
operator|+=
name|size
expr_stmt|;
block|}
do|while
condition|(
name|posn
operator|<
literal|20
condition|)
do|;
if|if
condition|(
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Serving %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|remote
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|has_sha1_file
argument_list|(
name|sha1
argument_list|)
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"git-ssh-push: could not find %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|remote
operator|=
operator|-
literal|1
expr_stmt|;
block|}
name|write
argument_list|(
name|fd_out
argument_list|,
operator|&
name|remote
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|remote
operator|<
literal|0
condition|)
return|return
literal|0
return|;
return|return
name|write_sha1_to_fd
argument_list|(
name|fd_out
argument_list|,
name|sha1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|serve_version
specifier|static
name|int
name|serve_version
parameter_list|(
name|int
name|fd_in
parameter_list|,
name|int
name|fd_out
parameter_list|)
block|{
if|if
condition|(
name|read
argument_list|(
name|fd_in
argument_list|,
operator|&
name|remote_version
argument_list|,
literal|1
argument_list|)
operator|<
literal|1
condition|)
return|return
operator|-
literal|1
return|;
name|write
argument_list|(
name|fd_out
argument_list|,
operator|&
name|local_version
argument_list|,
literal|1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|serve_ref
specifier|static
name|int
name|serve_ref
parameter_list|(
name|int
name|fd_in
parameter_list|,
name|int
name|fd_out
parameter_list|)
block|{
name|char
name|ref
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|posn
init|=
literal|0
decl_stmt|;
name|signed
name|char
name|remote
init|=
literal|0
decl_stmt|;
do|do
block|{
if|if
condition|(
name|read
argument_list|(
name|fd_in
argument_list|,
name|ref
operator|+
name|posn
argument_list|,
literal|1
argument_list|)
operator|<
literal|1
condition|)
return|return
operator|-
literal|1
return|;
name|posn
operator|++
expr_stmt|;
block|}
do|while
condition|(
name|ref
index|[
name|posn
operator|-
literal|1
index|]
condition|)
do|;
if|if
condition|(
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Serving %s\n"
argument_list|,
name|ref
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_ref_sha1
argument_list|(
name|ref
argument_list|,
name|sha1
argument_list|)
condition|)
name|remote
operator|=
operator|-
literal|1
expr_stmt|;
name|write
argument_list|(
name|fd_out
argument_list|,
operator|&
name|remote
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|remote
condition|)
return|return
literal|0
return|;
name|write
argument_list|(
name|fd_out
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|service
specifier|static
name|void
name|service
parameter_list|(
name|int
name|fd_in
parameter_list|,
name|int
name|fd_out
parameter_list|)
block|{
name|char
name|type
decl_stmt|;
name|int
name|retval
decl_stmt|;
do|do
block|{
name|retval
operator|=
name|read
argument_list|(
name|fd_in
argument_list|,
operator|&
name|type
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
name|retval
operator|<
literal|1
condition|)
block|{
if|if
condition|(
name|retval
operator|<
literal|0
condition|)
name|perror
argument_list|(
literal|"git-ssh-push: read "
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|type
operator|==
literal|'v'
operator|&&
name|serve_version
argument_list|(
name|fd_in
argument_list|,
name|fd_out
argument_list|)
condition|)
return|return;
if|if
condition|(
name|type
operator|==
literal|'o'
operator|&&
name|serve_object
argument_list|(
name|fd_in
argument_list|,
name|fd_out
argument_list|)
condition|)
return|return;
if|if
condition|(
name|type
operator|==
literal|'r'
operator|&&
name|serve_ref
argument_list|(
name|fd_in
argument_list|,
name|fd_out
argument_list|)
condition|)
return|return;
block|}
do|while
condition|(
literal|1
condition|)
do|;
block|}
end_function
begin_decl_stmt
DECL|variable|ssh_push_usage
specifier|static
specifier|const
name|char
name|ssh_push_usage
index|[]
init|=
literal|"git-ssh-push [-c] [-t] [-a] [-w ref] commit-id url"
decl_stmt|;
end_decl_stmt
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
name|int
name|arg
init|=
literal|1
decl_stmt|;
name|char
modifier|*
name|commit_id
decl_stmt|;
name|char
modifier|*
name|url
decl_stmt|;
name|int
name|fd_in
decl_stmt|,
name|fd_out
decl_stmt|;
specifier|const
name|char
modifier|*
name|prog
init|=
name|getenv
argument_list|(
literal|"GIT_SSH_PULL"
argument_list|)
condition|?
else|:
literal|"git-ssh-pull"
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|char
name|hex
index|[
literal|41
index|]
decl_stmt|;
while|while
condition|(
name|arg
operator|<
name|argc
operator|&&
name|argv
index|[
name|arg
index|]
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
name|argv
index|[
name|arg
index|]
index|[
literal|1
index|]
operator|==
literal|'w'
condition|)
name|arg
operator|++
expr_stmt|;
name|arg
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|argc
operator|<
name|arg
operator|+
literal|2
condition|)
name|usage
argument_list|(
name|ssh_push_usage
argument_list|)
expr_stmt|;
name|commit_id
operator|=
name|argv
index|[
name|arg
index|]
expr_stmt|;
name|url
operator|=
name|argv
index|[
name|arg
operator|+
literal|1
index|]
expr_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|commit_id
argument_list|,
name|sha1
argument_list|)
condition|)
name|usage
argument_list|(
name|ssh_push_usage
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
sizeof|sizeof
argument_list|(
name|hex
argument_list|)
argument_list|)
expr_stmt|;
name|argv
index|[
name|arg
index|]
operator|=
name|hex
expr_stmt|;
if|if
condition|(
name|setup_connection
argument_list|(
operator|&
name|fd_in
argument_list|,
operator|&
name|fd_out
argument_list|,
name|prog
argument_list|,
name|url
argument_list|,
name|arg
argument_list|,
name|argv
operator|+
literal|1
argument_list|)
condition|)
return|return
literal|1
return|;
name|service
argument_list|(
name|fd_in
argument_list|,
name|fd_out
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
