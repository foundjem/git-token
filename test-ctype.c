begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_function
DECL|function|test_isdigit
specifier|static
name|int
name|test_isdigit
parameter_list|(
name|int
name|c
parameter_list|)
block|{
return|return
name|isdigit
argument_list|(
name|c
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|test_isspace
specifier|static
name|int
name|test_isspace
parameter_list|(
name|int
name|c
parameter_list|)
block|{
return|return
name|isspace
argument_list|(
name|c
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|test_isalpha
specifier|static
name|int
name|test_isalpha
parameter_list|(
name|int
name|c
parameter_list|)
block|{
return|return
name|isalpha
argument_list|(
name|c
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|test_isalnum
specifier|static
name|int
name|test_isalnum
parameter_list|(
name|int
name|c
parameter_list|)
block|{
return|return
name|isalnum
argument_list|(
name|c
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|test_is_glob_special
specifier|static
name|int
name|test_is_glob_special
parameter_list|(
name|int
name|c
parameter_list|)
block|{
return|return
name|is_glob_special
argument_list|(
name|c
argument_list|)
return|;
block|}
end_function
begin_define
DECL|macro|DIGIT
define|#
directive|define
name|DIGIT
value|"0123456789"
end_define
begin_define
DECL|macro|LOWER
define|#
directive|define
name|LOWER
value|"abcdefghijklmnopqrstuvwxyz"
end_define
begin_define
DECL|macro|UPPER
define|#
directive|define
name|UPPER
value|"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
end_define
begin_struct
DECL|struct|ctype_class
specifier|static
specifier|const
struct|struct
name|ctype_class
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|test_fn
name|int
function_decl|(
modifier|*
name|test_fn
function_decl|)
parameter_list|(
name|int
parameter_list|)
function_decl|;
DECL|member|members
specifier|const
name|char
modifier|*
name|members
decl_stmt|;
block|}
DECL|variable|classes
name|classes
index|[]
init|=
block|{
block|{
literal|"isdigit"
block|,
name|test_isdigit
block|,
name|DIGIT
block|}
block|,
block|{
literal|"isspace"
block|,
name|test_isspace
block|,
literal|" \n\r\t"
block|}
block|,
block|{
literal|"isalpha"
block|,
name|test_isalpha
block|,
name|LOWER
name|UPPER
block|}
block|,
block|{
literal|"isalnum"
block|,
name|test_isalnum
block|,
name|LOWER
name|UPPER
name|DIGIT
block|}
block|,
block|{
literal|"is_glob_special"
block|,
name|test_is_glob_special
block|,
literal|"*?[\\"
block|}
block|,
block|{
name|NULL
block|}
block|}
struct|;
end_struct
begin_function
DECL|function|test_class
specifier|static
name|int
name|test_class
parameter_list|(
specifier|const
name|struct
name|ctype_class
modifier|*
name|test
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|rc
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|256
condition|;
name|i
operator|++
control|)
block|{
name|int
name|expected
init|=
name|i
condition|?
operator|!
operator|!
name|strchr
argument_list|(
name|test
operator|->
name|members
argument_list|,
name|i
argument_list|)
else|:
literal|0
decl_stmt|;
name|int
name|actual
init|=
name|test
operator|->
name|test_fn
argument_list|(
name|i
argument_list|)
decl_stmt|;
if|if
condition|(
name|actual
operator|!=
name|expected
condition|)
block|{
name|rc
operator|=
literal|1
expr_stmt|;
name|printf
argument_list|(
literal|"%s classifies char %d (0x%02x) wrongly\n"
argument_list|,
name|test
operator|->
name|name
argument_list|,
name|i
argument_list|,
name|i
argument_list|)
expr_stmt|;
block|}
block|}
return|return
name|rc
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
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
specifier|const
name|struct
name|ctype_class
modifier|*
name|test
decl_stmt|;
name|int
name|rc
init|=
literal|0
decl_stmt|;
for|for
control|(
name|test
operator|=
name|classes
init|;
name|test
operator|->
name|name
condition|;
name|test
operator|++
control|)
name|rc
operator||=
name|test_class
argument_list|(
name|test
argument_list|)
expr_stmt|;
return|return
name|rc
return|;
block|}
end_function
end_unit
