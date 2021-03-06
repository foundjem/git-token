begin_unit
begin_comment
comment|/*  * A git credential helper that interface with Windows' Credential Manager  *  */
end_comment
begin_include
include|#
directive|include
file|<windows.h>
end_include
begin_include
include|#
directive|include
file|<stdio.h>
end_include
begin_include
include|#
directive|include
file|<io.h>
end_include
begin_include
include|#
directive|include
file|<fcntl.h>
end_include
begin_comment
comment|/* common helpers */
end_comment
begin_define
DECL|macro|ARRAY_SIZE
define|#
directive|define
name|ARRAY_SIZE
parameter_list|(
name|x
parameter_list|)
value|(sizeof(x)/sizeof(x[0]))
end_define
begin_function
DECL|function|die
specifier|static
name|void
name|die
parameter_list|(
specifier|const
name|char
modifier|*
name|err
parameter_list|,
modifier|...
parameter_list|)
block|{
name|char
name|msg
index|[
literal|4096
index|]
decl_stmt|;
name|va_list
name|params
decl_stmt|;
name|va_start
argument_list|(
name|params
argument_list|,
name|err
argument_list|)
expr_stmt|;
name|vsnprintf
argument_list|(
name|msg
argument_list|,
sizeof|sizeof
argument_list|(
name|msg
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
literal|"%s\n"
argument_list|,
name|msg
argument_list|)
expr_stmt|;
name|va_end
argument_list|(
name|params
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|xmalloc
specifier|static
name|void
modifier|*
name|xmalloc
parameter_list|(
name|size_t
name|size
parameter_list|)
block|{
name|void
modifier|*
name|ret
init|=
name|malloc
argument_list|(
name|size
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|ret
operator|&&
operator|!
name|size
condition|)
name|ret
operator|=
name|malloc
argument_list|(
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ret
condition|)
name|die
argument_list|(
literal|"Out of memory"
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_comment
comment|/* MinGW doesn't have wincred.h, so we need to define stuff */
end_comment
begin_typedef
DECL|struct|_CREDENTIAL_ATTRIBUTEW
typedef|typedef
struct|struct
name|_CREDENTIAL_ATTRIBUTEW
block|{
DECL|member|Keyword
name|LPWSTR
name|Keyword
decl_stmt|;
DECL|member|Flags
name|DWORD
name|Flags
decl_stmt|;
DECL|member|ValueSize
name|DWORD
name|ValueSize
decl_stmt|;
DECL|member|Value
name|LPBYTE
name|Value
decl_stmt|;
block|}
DECL|typedef|CREDENTIAL_ATTRIBUTEW
DECL|typedef|PCREDENTIAL_ATTRIBUTEW
name|CREDENTIAL_ATTRIBUTEW
operator|,
typedef|*
name|PCREDENTIAL_ATTRIBUTEW
typedef|;
end_typedef
begin_typedef
DECL|struct|_CREDENTIALW
typedef|typedef
struct|struct
name|_CREDENTIALW
block|{
DECL|member|Flags
name|DWORD
name|Flags
decl_stmt|;
DECL|member|Type
name|DWORD
name|Type
decl_stmt|;
DECL|member|TargetName
name|LPWSTR
name|TargetName
decl_stmt|;
DECL|member|Comment
name|LPWSTR
name|Comment
decl_stmt|;
DECL|member|LastWritten
name|FILETIME
name|LastWritten
decl_stmt|;
DECL|member|CredentialBlobSize
name|DWORD
name|CredentialBlobSize
decl_stmt|;
DECL|member|CredentialBlob
name|LPBYTE
name|CredentialBlob
decl_stmt|;
DECL|member|Persist
name|DWORD
name|Persist
decl_stmt|;
DECL|member|AttributeCount
name|DWORD
name|AttributeCount
decl_stmt|;
DECL|member|Attributes
name|PCREDENTIAL_ATTRIBUTEW
name|Attributes
decl_stmt|;
DECL|member|TargetAlias
name|LPWSTR
name|TargetAlias
decl_stmt|;
DECL|member|UserName
name|LPWSTR
name|UserName
decl_stmt|;
block|}
DECL|typedef|CREDENTIALW
DECL|typedef|PCREDENTIALW
name|CREDENTIALW
operator|,
typedef|*
name|PCREDENTIALW
typedef|;
end_typedef
begin_define
DECL|macro|CRED_TYPE_GENERIC
define|#
directive|define
name|CRED_TYPE_GENERIC
value|1
end_define
begin_define
DECL|macro|CRED_PERSIST_LOCAL_MACHINE
define|#
directive|define
name|CRED_PERSIST_LOCAL_MACHINE
value|2
end_define
begin_define
DECL|macro|CRED_MAX_ATTRIBUTES
define|#
directive|define
name|CRED_MAX_ATTRIBUTES
value|64
end_define
begin_typedef
DECL|typedef|CredWriteWT
typedef|typedef
name|BOOL
function_decl|(
name|WINAPI
modifier|*
name|CredWriteWT
function_decl|)
parameter_list|(
name|PCREDENTIALW
parameter_list|,
name|DWORD
parameter_list|)
function_decl|;
end_typedef
begin_typedef
DECL|typedef|CredEnumerateWT
typedef|typedef
name|BOOL
function_decl|(
name|WINAPI
modifier|*
name|CredEnumerateWT
function_decl|)
parameter_list|(
name|LPCWSTR
parameter_list|,
name|DWORD
parameter_list|,
name|DWORD
modifier|*
parameter_list|,
name|PCREDENTIALW
modifier|*
modifier|*
parameter_list|)
function_decl|;
end_typedef
begin_typedef
DECL|typedef|CredFreeT
typedef|typedef
name|VOID
function_decl|(
name|WINAPI
modifier|*
name|CredFreeT
function_decl|)
parameter_list|(
name|PVOID
parameter_list|)
function_decl|;
end_typedef
begin_typedef
DECL|typedef|CredDeleteWT
typedef|typedef
name|BOOL
function_decl|(
name|WINAPI
modifier|*
name|CredDeleteWT
function_decl|)
parameter_list|(
name|LPCWSTR
parameter_list|,
name|DWORD
parameter_list|,
name|DWORD
parameter_list|)
function_decl|;
end_typedef
begin_decl_stmt
DECL|variable|advapi
specifier|static
name|HMODULE
name|advapi
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|CredWriteW
specifier|static
name|CredWriteWT
name|CredWriteW
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|CredEnumerateW
specifier|static
name|CredEnumerateWT
name|CredEnumerateW
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|CredFree
specifier|static
name|CredFreeT
name|CredFree
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|CredDeleteW
specifier|static
name|CredDeleteWT
name|CredDeleteW
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|load_cred_funcs
specifier|static
name|void
name|load_cred_funcs
parameter_list|(
name|void
parameter_list|)
block|{
comment|/* load DLLs */
name|advapi
operator|=
name|LoadLibrary
argument_list|(
literal|"advapi32.dll"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|advapi
condition|)
name|die
argument_list|(
literal|"failed to load advapi32.dll"
argument_list|)
expr_stmt|;
comment|/* get function pointers */
name|CredWriteW
operator|=
operator|(
name|CredWriteWT
operator|)
name|GetProcAddress
argument_list|(
name|advapi
argument_list|,
literal|"CredWriteW"
argument_list|)
expr_stmt|;
name|CredEnumerateW
operator|=
operator|(
name|CredEnumerateWT
operator|)
name|GetProcAddress
argument_list|(
name|advapi
argument_list|,
literal|"CredEnumerateW"
argument_list|)
expr_stmt|;
name|CredFree
operator|=
operator|(
name|CredFreeT
operator|)
name|GetProcAddress
argument_list|(
name|advapi
argument_list|,
literal|"CredFree"
argument_list|)
expr_stmt|;
name|CredDeleteW
operator|=
operator|(
name|CredDeleteWT
operator|)
name|GetProcAddress
argument_list|(
name|advapi
argument_list|,
literal|"CredDeleteW"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|CredWriteW
operator|||
operator|!
name|CredEnumerateW
operator|||
operator|!
name|CredFree
operator|||
operator|!
name|CredDeleteW
condition|)
name|die
argument_list|(
literal|"failed to load functions"
argument_list|)
expr_stmt|;
block|}
end_function
begin_decl_stmt
DECL|variable|wusername
DECL|variable|password
DECL|variable|protocol
DECL|variable|host
DECL|variable|path
DECL|variable|target
specifier|static
name|WCHAR
modifier|*
name|wusername
decl_stmt|,
modifier|*
name|password
decl_stmt|,
modifier|*
name|protocol
decl_stmt|,
modifier|*
name|host
decl_stmt|,
modifier|*
name|path
decl_stmt|,
name|target
index|[
literal|1024
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|write_item
specifier|static
name|void
name|write_item
parameter_list|(
specifier|const
name|char
modifier|*
name|what
parameter_list|,
name|LPCWSTR
name|wbuf
parameter_list|,
name|int
name|wlen
parameter_list|)
block|{
name|char
modifier|*
name|buf
decl_stmt|;
name|int
name|len
init|=
name|WideCharToMultiByte
argument_list|(
name|CP_UTF8
argument_list|,
literal|0
argument_list|,
name|wbuf
argument_list|,
name|wlen
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|,
name|FALSE
argument_list|)
decl_stmt|;
name|buf
operator|=
name|xmalloc
argument_list|(
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|WideCharToMultiByte
argument_list|(
name|CP_UTF8
argument_list|,
literal|0
argument_list|,
name|wbuf
argument_list|,
name|wlen
argument_list|,
name|buf
argument_list|,
name|len
argument_list|,
name|NULL
argument_list|,
name|FALSE
argument_list|)
condition|)
name|die
argument_list|(
literal|"WideCharToMultiByte failed!"
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s="
argument_list|,
name|what
argument_list|)
expr_stmt|;
name|fwrite
argument_list|(
name|buf
argument_list|,
literal|1
argument_list|,
name|len
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Match an (optional) expected string and a delimiter in the target string,  * consuming the matched text by updating the target pointer.  */
end_comment
begin_function
DECL|function|wcsstr_last
specifier|static
name|LPCWSTR
name|wcsstr_last
parameter_list|(
name|LPCWSTR
name|str
parameter_list|,
name|LPCWSTR
name|find
parameter_list|)
block|{
name|LPCWSTR
name|res
init|=
name|NULL
decl_stmt|,
name|pos
decl_stmt|;
for|for
control|(
name|pos
operator|=
name|wcsstr
argument_list|(
name|str
argument_list|,
name|find
argument_list|)
init|;
name|pos
condition|;
name|pos
operator|=
name|wcsstr
argument_list|(
name|pos
operator|+
literal|1
argument_list|,
name|find
argument_list|)
control|)
name|res
operator|=
name|pos
expr_stmt|;
return|return
name|res
return|;
block|}
end_function
begin_function
DECL|function|match_part_with_last
specifier|static
name|int
name|match_part_with_last
parameter_list|(
name|LPCWSTR
modifier|*
name|ptarget
parameter_list|,
name|LPCWSTR
name|want
parameter_list|,
name|LPCWSTR
name|delim
parameter_list|,
name|int
name|last
parameter_list|)
block|{
name|LPCWSTR
name|delim_pos
decl_stmt|,
name|start
init|=
operator|*
name|ptarget
decl_stmt|;
name|int
name|len
decl_stmt|;
comment|/* find start of delimiter (or end-of-string if delim is empty) */
if|if
condition|(
operator|*
name|delim
condition|)
name|delim_pos
operator|=
name|last
condition|?
name|wcsstr_last
argument_list|(
name|start
argument_list|,
name|delim
argument_list|)
else|:
name|wcsstr
argument_list|(
name|start
argument_list|,
name|delim
argument_list|)
expr_stmt|;
else|else
name|delim_pos
operator|=
name|start
operator|+
name|wcslen
argument_list|(
name|start
argument_list|)
expr_stmt|;
comment|/* 	 * match text up to delimiter, or end of string (e.g. the '/' after 	 * host is optional if not followed by a path) 	 */
if|if
condition|(
name|delim_pos
condition|)
name|len
operator|=
name|delim_pos
operator|-
name|start
expr_stmt|;
else|else
name|len
operator|=
name|wcslen
argument_list|(
name|start
argument_list|)
expr_stmt|;
comment|/* update ptarget if we either found a delimiter or need a match */
if|if
condition|(
name|delim_pos
operator|||
name|want
condition|)
operator|*
name|ptarget
operator|=
name|delim_pos
condition|?
name|delim_pos
operator|+
name|wcslen
argument_list|(
name|delim
argument_list|)
else|:
name|start
operator|+
name|len
expr_stmt|;
return|return
operator|!
name|want
operator|||
operator|(
operator|!
name|wcsncmp
argument_list|(
name|want
argument_list|,
name|start
argument_list|,
name|len
argument_list|)
operator|&&
operator|!
name|want
index|[
name|len
index|]
operator|)
return|;
block|}
end_function
begin_function
DECL|function|match_part
specifier|static
name|int
name|match_part
parameter_list|(
name|LPCWSTR
modifier|*
name|ptarget
parameter_list|,
name|LPCWSTR
name|want
parameter_list|,
name|LPCWSTR
name|delim
parameter_list|)
block|{
return|return
name|match_part_with_last
argument_list|(
name|ptarget
argument_list|,
name|want
argument_list|,
name|delim
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|match_part_last
specifier|static
name|int
name|match_part_last
parameter_list|(
name|LPCWSTR
modifier|*
name|ptarget
parameter_list|,
name|LPCWSTR
name|want
parameter_list|,
name|LPCWSTR
name|delim
parameter_list|)
block|{
return|return
name|match_part_with_last
argument_list|(
name|ptarget
argument_list|,
name|want
argument_list|,
name|delim
argument_list|,
literal|1
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|match_cred
specifier|static
name|int
name|match_cred
parameter_list|(
specifier|const
name|CREDENTIALW
modifier|*
name|cred
parameter_list|)
block|{
name|LPCWSTR
name|target
init|=
name|cred
operator|->
name|TargetName
decl_stmt|;
if|if
condition|(
name|wusername
operator|&&
name|wcscmp
argument_list|(
name|wusername
argument_list|,
name|cred
operator|->
name|UserName
argument_list|)
condition|)
return|return
literal|0
return|;
return|return
name|match_part
argument_list|(
operator|&
name|target
argument_list|,
literal|L"git"
argument_list|,
literal|L":"
argument_list|)
operator|&&
name|match_part
argument_list|(
operator|&
name|target
argument_list|,
name|protocol
argument_list|,
literal|L"://"
argument_list|)
operator|&&
name|match_part_last
argument_list|(
operator|&
name|target
argument_list|,
name|wusername
argument_list|,
literal|L"@"
argument_list|)
operator|&&
name|match_part
argument_list|(
operator|&
name|target
argument_list|,
name|host
argument_list|,
literal|L"/"
argument_list|)
operator|&&
name|match_part
argument_list|(
operator|&
name|target
argument_list|,
name|path
argument_list|,
literal|L""
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|get_credential
specifier|static
name|void
name|get_credential
parameter_list|(
name|void
parameter_list|)
block|{
name|CREDENTIALW
modifier|*
modifier|*
name|creds
decl_stmt|;
name|DWORD
name|num_creds
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|CredEnumerateW
argument_list|(
literal|L"git:*"
argument_list|,
literal|0
argument_list|,
operator|&
name|num_creds
argument_list|,
operator|&
name|creds
argument_list|)
condition|)
return|return;
comment|/* search for the first credential that matches username */
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_creds
condition|;
operator|++
name|i
control|)
if|if
condition|(
name|match_cred
argument_list|(
name|creds
index|[
name|i
index|]
argument_list|)
condition|)
block|{
name|write_item
argument_list|(
literal|"username"
argument_list|,
name|creds
index|[
name|i
index|]
operator|->
name|UserName
argument_list|,
name|wcslen
argument_list|(
name|creds
index|[
name|i
index|]
operator|->
name|UserName
argument_list|)
argument_list|)
expr_stmt|;
name|write_item
argument_list|(
literal|"password"
argument_list|,
operator|(
name|LPCWSTR
operator|)
name|creds
index|[
name|i
index|]
operator|->
name|CredentialBlob
argument_list|,
name|creds
index|[
name|i
index|]
operator|->
name|CredentialBlobSize
operator|/
sizeof|sizeof
argument_list|(
name|WCHAR
argument_list|)
argument_list|)
expr_stmt|;
break|break;
block|}
name|CredFree
argument_list|(
name|creds
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
name|void
parameter_list|)
block|{
name|CREDENTIALW
name|cred
decl_stmt|;
if|if
condition|(
operator|!
name|wusername
operator|||
operator|!
name|password
condition|)
return|return;
name|cred
operator|.
name|Flags
operator|=
literal|0
expr_stmt|;
name|cred
operator|.
name|Type
operator|=
name|CRED_TYPE_GENERIC
expr_stmt|;
name|cred
operator|.
name|TargetName
operator|=
name|target
expr_stmt|;
name|cred
operator|.
name|Comment
operator|=
literal|L"saved by git-credential-wincred"
expr_stmt|;
name|cred
operator|.
name|CredentialBlobSize
operator|=
operator|(
name|wcslen
argument_list|(
name|password
argument_list|)
operator|)
operator|*
sizeof|sizeof
argument_list|(
name|WCHAR
argument_list|)
expr_stmt|;
name|cred
operator|.
name|CredentialBlob
operator|=
operator|(
name|LPVOID
operator|)
name|password
expr_stmt|;
name|cred
operator|.
name|Persist
operator|=
name|CRED_PERSIST_LOCAL_MACHINE
expr_stmt|;
name|cred
operator|.
name|AttributeCount
operator|=
literal|0
expr_stmt|;
name|cred
operator|.
name|Attributes
operator|=
name|NULL
expr_stmt|;
name|cred
operator|.
name|TargetAlias
operator|=
name|NULL
expr_stmt|;
name|cred
operator|.
name|UserName
operator|=
name|wusername
expr_stmt|;
if|if
condition|(
operator|!
name|CredWriteW
argument_list|(
operator|&
name|cred
argument_list|,
literal|0
argument_list|)
condition|)
name|die
argument_list|(
literal|"CredWrite failed"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|erase_credential
specifier|static
name|void
name|erase_credential
parameter_list|(
name|void
parameter_list|)
block|{
name|CREDENTIALW
modifier|*
modifier|*
name|creds
decl_stmt|;
name|DWORD
name|num_creds
decl_stmt|;
name|int
name|i
decl_stmt|;
if|if
condition|(
operator|!
name|CredEnumerateW
argument_list|(
literal|L"git:*"
argument_list|,
literal|0
argument_list|,
operator|&
name|num_creds
argument_list|,
operator|&
name|creds
argument_list|)
condition|)
return|return;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_creds
condition|;
operator|++
name|i
control|)
block|{
if|if
condition|(
name|match_cred
argument_list|(
name|creds
index|[
name|i
index|]
argument_list|)
condition|)
name|CredDeleteW
argument_list|(
name|creds
index|[
name|i
index|]
operator|->
name|TargetName
argument_list|,
name|creds
index|[
name|i
index|]
operator|->
name|Type
argument_list|,
literal|0
argument_list|)
expr_stmt|;
block|}
name|CredFree
argument_list|(
name|creds
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|utf8_to_utf16_dup
specifier|static
name|WCHAR
modifier|*
name|utf8_to_utf16_dup
parameter_list|(
specifier|const
name|char
modifier|*
name|str
parameter_list|)
block|{
name|int
name|wlen
init|=
name|MultiByteToWideChar
argument_list|(
name|CP_UTF8
argument_list|,
literal|0
argument_list|,
name|str
argument_list|,
operator|-
literal|1
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
decl_stmt|;
name|WCHAR
modifier|*
name|wstr
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|WCHAR
argument_list|)
operator|*
name|wlen
argument_list|)
decl_stmt|;
name|MultiByteToWideChar
argument_list|(
name|CP_UTF8
argument_list|,
literal|0
argument_list|,
name|str
argument_list|,
operator|-
literal|1
argument_list|,
name|wstr
argument_list|,
name|wlen
argument_list|)
expr_stmt|;
return|return
name|wstr
return|;
block|}
end_function
begin_function
DECL|function|read_credential
specifier|static
name|void
name|read_credential
parameter_list|(
name|void
parameter_list|)
block|{
name|char
name|buf
index|[
literal|1024
index|]
decl_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
name|stdin
argument_list|)
condition|)
block|{
name|char
modifier|*
name|v
decl_stmt|;
name|int
name|len
init|=
name|strlen
argument_list|(
name|buf
argument_list|)
decl_stmt|;
comment|/* strip trailing CR / LF */
while|while
condition|(
name|len
operator|&&
name|strchr
argument_list|(
literal|"\r\n"
argument_list|,
name|buf
index|[
name|len
operator|-
literal|1
index|]
argument_list|)
condition|)
name|buf
index|[
operator|--
name|len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
operator|*
name|buf
condition|)
break|break;
name|v
operator|=
name|strchr
argument_list|(
name|buf
argument_list|,
literal|'='
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|v
condition|)
name|die
argument_list|(
literal|"bad input: %s"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
operator|*
name|v
operator|++
operator|=
literal|'\0'
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"protocol"
argument_list|)
condition|)
name|protocol
operator|=
name|utf8_to_utf16_dup
argument_list|(
name|v
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"host"
argument_list|)
condition|)
name|host
operator|=
name|utf8_to_utf16_dup
argument_list|(
name|v
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"path"
argument_list|)
condition|)
name|path
operator|=
name|utf8_to_utf16_dup
argument_list|(
name|v
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"username"
argument_list|)
condition|)
block|{
name|wusername
operator|=
name|utf8_to_utf16_dup
argument_list|(
name|v
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|buf
argument_list|,
literal|"password"
argument_list|)
condition|)
name|password
operator|=
name|utf8_to_utf16_dup
argument_list|(
name|v
argument_list|)
expr_stmt|;
else|else
name|die
argument_list|(
literal|"unrecognized input"
argument_list|)
expr_stmt|;
block|}
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
name|argv
index|[]
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|usage
init|=
literal|"usage: git credential-wincred<get|store|erase>\n"
decl_stmt|;
if|if
condition|(
operator|!
name|argv
index|[
literal|1
index|]
condition|)
name|die
argument_list|(
name|usage
argument_list|)
expr_stmt|;
comment|/* git use binary pipes to avoid CRLF-issues */
name|_setmode
argument_list|(
name|_fileno
argument_list|(
name|stdin
argument_list|)
argument_list|,
name|_O_BINARY
argument_list|)
expr_stmt|;
name|_setmode
argument_list|(
name|_fileno
argument_list|(
name|stdout
argument_list|)
argument_list|,
name|_O_BINARY
argument_list|)
expr_stmt|;
name|read_credential
argument_list|()
expr_stmt|;
name|load_cred_funcs
argument_list|()
expr_stmt|;
if|if
condition|(
operator|!
name|protocol
operator|||
operator|!
operator|(
name|host
operator|||
name|path
operator|)
condition|)
return|return
literal|0
return|;
comment|/* prepare 'target', the unique key for the credential */
name|wcscpy
argument_list|(
name|target
argument_list|,
literal|L"git:"
argument_list|)
expr_stmt|;
name|wcsncat
argument_list|(
name|target
argument_list|,
name|protocol
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|target
argument_list|)
argument_list|)
expr_stmt|;
name|wcsncat
argument_list|(
name|target
argument_list|,
literal|L"://"
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|target
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|wusername
condition|)
block|{
name|wcsncat
argument_list|(
name|target
argument_list|,
name|wusername
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|target
argument_list|)
argument_list|)
expr_stmt|;
name|wcsncat
argument_list|(
name|target
argument_list|,
literal|L"@"
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|target
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|host
condition|)
name|wcsncat
argument_list|(
name|target
argument_list|,
name|host
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|target
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|path
condition|)
block|{
name|wcsncat
argument_list|(
name|target
argument_list|,
literal|L"/"
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|target
argument_list|)
argument_list|)
expr_stmt|;
name|wcsncat
argument_list|(
name|target
argument_list|,
name|path
argument_list|,
name|ARRAY_SIZE
argument_list|(
name|target
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"get"
argument_list|)
condition|)
name|get_credential
argument_list|()
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"store"
argument_list|)
condition|)
name|store_credential
argument_list|()
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
literal|1
index|]
argument_list|,
literal|"erase"
argument_list|)
condition|)
name|erase_credential
argument_list|()
expr_stmt|;
comment|/* otherwise, ignore unknown action */
return|return
literal|0
return|;
block|}
end_function
end_unit
