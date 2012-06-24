begin_unit
begin_include
include|#
directive|include
file|"git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"credential.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_decl_stmt
DECL|variable|usage_msg
specifier|static
specifier|const
name|char
name|usage_msg
index|[]
init|=
literal|"git credential [fill|approve|reject]"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|cmd_credential
name|int
name|cmd_credential
parameter_list|(
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
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
name|op
operator|=
name|argv
index|[
literal|1
index|]
expr_stmt|;
if|if
condition|(
operator|!
name|op
condition|)
name|usage
argument_list|(
name|usage_msg
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
literal|"unable to read credential from stdin"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|op
argument_list|,
literal|"fill"
argument_list|)
condition|)
block|{
name|credential_fill
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
name|credential_write
argument_list|(
operator|&
name|c
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|op
argument_list|,
literal|"approve"
argument_list|)
condition|)
block|{
name|credential_approve
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|op
argument_list|,
literal|"reject"
argument_list|)
condition|)
block|{
name|credential_reject
argument_list|(
operator|&
name|c
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|usage
argument_list|(
name|usage_msg
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
