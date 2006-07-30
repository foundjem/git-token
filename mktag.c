begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_comment
comment|/*  * A signature file has a very simple fixed format: four lines  * of "object<sha1>" + "type<typename>" + "tag<tagname>" +  * "tagger<committer>", followed by a blank line, a free-form tag  * message and a signature block that git itself doesn't care about,  * but that can be verified with gpg or similar.  *  * The first three lines are guaranteed to be at least 63 bytes:  * "object<sha1>\n" is 48 bytes, "type tag\n" at 9 bytes is the  * shortest possible type-line, and "tag .\n" at 6 bytes is the  * shortest single-character-tag line.   *  * We also artificially limit the size of the full object to 8kB.  * Just because I'm a lazy bastard, and if you can't fit a signature  * in that size, you're doing something wrong.  */
end_comment
begin_comment
comment|/* Some random size */
end_comment
begin_define
DECL|macro|MAXSIZE
define|#
directive|define
name|MAXSIZE
value|(8192)
end_define
begin_comment
comment|/*  * We refuse to tag something we can't verify. Just because.  */
end_comment
begin_function
DECL|function|verify_object
specifier|static
name|int
name|verify_object
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|expected_type
parameter_list|)
block|{
name|int
name|ret
init|=
operator|-
literal|1
decl_stmt|;
name|char
name|type
index|[
literal|100
index|]
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|void
modifier|*
name|buffer
init|=
name|read_sha1_file
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
name|buffer
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|type
argument_list|,
name|expected_type
argument_list|)
condition|)
name|ret
operator|=
name|check_sha1_signature
argument_list|(
name|sha1
argument_list|,
name|buffer
argument_list|,
name|size
argument_list|,
name|type
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
block|}
return|return
name|ret
return|;
block|}
end_function
begin_ifdef
ifdef|#
directive|ifdef
name|NO_C99_FORMAT
end_ifdef
begin_define
DECL|macro|PD_FMT
define|#
directive|define
name|PD_FMT
value|"%d"
end_define
begin_else
else|#
directive|else
end_else
begin_define
DECL|macro|PD_FMT
define|#
directive|define
name|PD_FMT
value|"%td"
end_define
begin_endif
endif|#
directive|endif
end_endif
begin_function
DECL|function|verify_tag
specifier|static
name|int
name|verify_tag
parameter_list|(
name|char
modifier|*
name|buffer
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|)
block|{
name|int
name|typelen
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|object
decl_stmt|,
modifier|*
name|type_line
decl_stmt|,
modifier|*
name|tag_line
decl_stmt|,
modifier|*
name|tagger_line
decl_stmt|;
if|if
condition|(
name|size
operator|<
literal|64
condition|)
return|return
name|error
argument_list|(
literal|"wanna fool me ? you obviously got the size wrong !"
argument_list|)
return|;
name|buffer
index|[
name|size
index|]
operator|=
literal|0
expr_stmt|;
comment|/* Verify object line */
name|object
operator|=
name|buffer
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|object
argument_list|,
literal|"object "
argument_list|,
literal|7
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"char%d: does not start with \"object \""
argument_list|,
literal|0
argument_list|)
return|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|object
operator|+
literal|7
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"char%d: could not get SHA1 hash"
argument_list|,
literal|7
argument_list|)
return|;
comment|/* Verify type line */
name|type_line
operator|=
name|object
operator|+
literal|48
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|type_line
operator|-
literal|1
argument_list|,
literal|"\ntype "
argument_list|,
literal|6
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"char%d: could not find \"\\ntype \""
argument_list|,
literal|47
argument_list|)
return|;
comment|/* Verify tag-line */
name|tag_line
operator|=
name|strchr
argument_list|(
name|type_line
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tag_line
condition|)
return|return
name|error
argument_list|(
literal|"char"
name|PD_FMT
literal|": could not find next \"\\n\""
argument_list|,
name|type_line
operator|-
name|buffer
argument_list|)
return|;
name|tag_line
operator|++
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|tag_line
argument_list|,
literal|"tag "
argument_list|,
literal|4
argument_list|)
operator|||
name|tag_line
index|[
literal|4
index|]
operator|==
literal|'\n'
condition|)
return|return
name|error
argument_list|(
literal|"char"
name|PD_FMT
literal|": no \"tag \" found"
argument_list|,
name|tag_line
operator|-
name|buffer
argument_list|)
return|;
comment|/* Get the actual type */
name|typelen
operator|=
name|tag_line
operator|-
name|type_line
operator|-
name|strlen
argument_list|(
literal|"type \n"
argument_list|)
expr_stmt|;
if|if
condition|(
name|typelen
operator|>=
sizeof|sizeof
argument_list|(
name|type
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"char"
name|PD_FMT
literal|": type too long"
argument_list|,
name|type_line
operator|+
literal|5
operator|-
name|buffer
argument_list|)
return|;
name|memcpy
argument_list|(
name|type
argument_list|,
name|type_line
operator|+
literal|5
argument_list|,
name|typelen
argument_list|)
expr_stmt|;
name|type
index|[
name|typelen
index|]
operator|=
literal|0
expr_stmt|;
comment|/* Verify that the object matches */
if|if
condition|(
name|verify_object
argument_list|(
name|sha1
argument_list|,
name|type
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"char%d: could not verify object %s"
argument_list|,
literal|7
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
return|;
comment|/* Verify the tag-name: we don't allow control characters or spaces in it */
name|tag_line
operator|+=
literal|4
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|unsigned
name|char
name|c
init|=
operator|*
name|tag_line
operator|++
decl_stmt|;
if|if
condition|(
name|c
operator|==
literal|'\n'
condition|)
break|break;
if|if
condition|(
name|c
operator|>
literal|' '
condition|)
continue|continue;
return|return
name|error
argument_list|(
literal|"char"
name|PD_FMT
literal|": could not verify tag name"
argument_list|,
name|tag_line
operator|-
name|buffer
argument_list|)
return|;
block|}
comment|/* Verify the tagger line */
name|tagger_line
operator|=
name|tag_line
expr_stmt|;
if|if
condition|(
name|memcmp
argument_list|(
name|tagger_line
argument_list|,
literal|"tagger"
argument_list|,
literal|6
argument_list|)
operator|||
operator|(
name|tagger_line
index|[
literal|6
index|]
operator|==
literal|'\n'
operator|)
condition|)
return|return
name|error
argument_list|(
literal|"char"
name|PD_FMT
literal|": could not find \"tagger\""
argument_list|,
name|tagger_line
operator|-
name|buffer
argument_list|)
return|;
comment|/* TODO: check for committer info + blank line? */
comment|/* Also, the minimum length is probably + "tagger .", or 63+8=71 */
comment|/* The actual stuff afterwards we don't care about.. */
return|return
literal|0
return|;
block|}
end_function
begin_undef
DECL|macro|PD_FMT
undef|#
directive|undef
name|PD_FMT
end_undef
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
name|unsigned
name|long
name|size
init|=
literal|4096
decl_stmt|;
name|char
modifier|*
name|buffer
init|=
name|malloc
argument_list|(
name|size
argument_list|)
decl_stmt|;
name|unsigned
name|char
name|result_sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|argc
operator|!=
literal|1
condition|)
name|usage
argument_list|(
literal|"cat<signaturefile> | git-mktag"
argument_list|)
expr_stmt|;
name|setup_git_directory
argument_list|()
expr_stmt|;
if|if
condition|(
name|read_pipe
argument_list|(
literal|0
argument_list|,
operator|&
name|buffer
argument_list|,
operator|&
name|size
argument_list|)
condition|)
block|{
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"could not read from stdin"
argument_list|)
expr_stmt|;
block|}
comment|/* Verify it for some basic sanity: it needs to start with 	   "object<sha1>\ntype\ntagger " */
if|if
condition|(
name|verify_tag
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"invalid tag signature file"
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_sha1_file
argument_list|(
name|buffer
argument_list|,
name|size
argument_list|,
name|tag_type
argument_list|,
name|result_sha1
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to write tag file"
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buffer
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|result_sha1
argument_list|)
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
end_unit
