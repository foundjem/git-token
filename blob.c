begin_unit
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|<stdlib.h>
end_include
begin_decl_stmt
DECL|variable|blob_type
specifier|const
name|char
modifier|*
name|blob_type
init|=
literal|"blob"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|lookup_blob
name|struct
name|blob
modifier|*
name|lookup_blob
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|lookup_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|obj
condition|)
block|{
name|struct
name|blob
modifier|*
name|ret
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|blob
argument_list|)
argument_list|)
decl_stmt|;
name|memset
argument_list|(
name|ret
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|blob
argument_list|)
argument_list|)
expr_stmt|;
name|created_object
argument_list|(
name|sha1
argument_list|,
operator|&
name|ret
operator|->
name|object
argument_list|)
expr_stmt|;
name|ret
operator|->
name|object
operator|.
name|type
operator|=
name|blob_type
expr_stmt|;
return|return
name|ret
return|;
block|}
if|if
condition|(
name|obj
operator|->
name|parsed
operator|&&
name|obj
operator|->
name|type
operator|!=
name|blob_type
condition|)
block|{
name|error
argument_list|(
literal|"Object %s is a %s, not a blob"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|,
name|obj
operator|->
name|type
argument_list|)
expr_stmt|;
return|return
name|NULL
return|;
block|}
return|return
operator|(
expr|struct
name|blob
operator|*
operator|)
name|obj
return|;
block|}
end_function
end_unit
