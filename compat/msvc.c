begin_unit
begin_include
include|#
directive|include
file|"../git-compat-util.h"
end_include
begin_include
include|#
directive|include
file|"win32.h"
end_include
begin_include
include|#
directive|include
file|<conio.h>
end_include
begin_include
include|#
directive|include
file|"../strbuf.h"
end_include
begin_function
DECL|function|opendir
name|DIR
modifier|*
name|opendir
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|int
name|len
decl_stmt|;
name|DIR
modifier|*
name|p
decl_stmt|;
name|p
operator|=
operator|(
name|DIR
operator|*
operator|)
name|malloc
argument_list|(
sizeof|sizeof
argument_list|(
name|DIR
argument_list|)
argument_list|)
expr_stmt|;
name|memset
argument_list|(
name|p
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|DIR
argument_list|)
argument_list|)
expr_stmt|;
name|strncpy
argument_list|(
name|p
operator|->
name|dd_name
argument_list|,
name|name
argument_list|,
name|PATH_MAX
argument_list|)
expr_stmt|;
name|len
operator|=
name|strlen
argument_list|(
name|p
operator|->
name|dd_name
argument_list|)
expr_stmt|;
name|p
operator|->
name|dd_name
index|[
name|len
index|]
operator|=
literal|'/'
expr_stmt|;
name|p
operator|->
name|dd_name
index|[
name|len
operator|+
literal|1
index|]
operator|=
literal|'*'
expr_stmt|;
if|if
condition|(
name|p
operator|==
name|NULL
condition|)
return|return
name|NULL
return|;
name|p
operator|->
name|dd_handle
operator|=
name|_findfirst
argument_list|(
name|p
operator|->
name|dd_name
argument_list|,
operator|&
name|p
operator|->
name|dd_dta
argument_list|)
expr_stmt|;
if|if
condition|(
name|p
operator|->
name|dd_handle
operator|==
operator|-
literal|1
condition|)
block|{
name|free
argument_list|(
name|p
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
return|return
name|p
return|;
block|}
end_function
begin_function
DECL|function|closedir
name|int
name|closedir
parameter_list|(
name|DIR
modifier|*
name|dir
parameter_list|)
block|{
name|_findclose
argument_list|(
name|dir
operator|->
name|dd_handle
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|dir
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_include
include|#
directive|include
file|"mingw.c"
end_include
end_unit
