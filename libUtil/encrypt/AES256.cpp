#include "AES256.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <bcrypt.h>
#include <wincrypt.h>
#pragma comment(lib, "bcrypt.lib")
#pragma comment(lib, "crypt32.lib")
#else
#include <openssl/evp.h>
#include <openssl/rand.h>
#endif
namespace nsUtil
{
#define AES_SALT_LEN    8
#define AES_KEY_LEN     32
#define AES_IV_LEN      16
#define AES_PBKDF2_ITER 10000

static const unsigned char AES_MAGIC[8] = { 'S','a','l','t','e','d','_','_' };

#ifdef _MSC_VER // >> claude-code 2026-09-18 : Windows cross-compile (CNG/Crypt32 impl)
static char* aes_b64_encode(const unsigned char* in, int inlen)
{
	DWORD outlen = 0;
	if(!CryptBinaryToStringA(in, (DWORD)inlen, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, NULL, &outlen))
		return NULL;
	char* out = (char*)malloc(outlen + 1);
	if(!out) return NULL;
	if(!CryptBinaryToStringA(in, (DWORD)inlen, CRYPT_STRING_BASE64 | CRYPT_STRING_NOCRLF, out, &outlen))
	{
		free(out);
		return NULL;
	}
	out[outlen] = '\0';
	return out;
}

static unsigned char* aes_b64_decode(const char* in, int* outlen)
{
	DWORD binlen = 0;
	if(!CryptStringToBinaryA(in, 0, CRYPT_STRING_BASE64, NULL, &binlen, NULL, NULL))
		return NULL;
	unsigned char* out = (unsigned char*)malloc(binlen > 0 ? binlen : 1);
	if(!out) return NULL;
	if(!CryptStringToBinaryA(in, 0, CRYPT_STRING_BASE64, out, &binlen, NULL, NULL))
	{
		free(out);
		return NULL;
	}
	*outlen = (int)binlen;
	return out;
}

static int aes_rand_bytes(unsigned char* buf, int len)
{
	return BCryptGenRandom(NULL, buf, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG) == 0 ? 1 : 0;
}

// matches OpenSSL's PKCS5_PBKDF2_HMAC(..., EVP_sha256(), ...) -- returns 1 on success, 0 on failure
static int aes_derive_keyiv(KCSTR pass, int passlen, const unsigned char* salt, int saltlen,
							 int iter, unsigned char* out, int outlen)
{
	BCRYPT_ALG_HANDLE hAlg = NULL;
	int ok = 0;
	if(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, NULL, BCRYPT_ALG_HANDLE_HMAC_FLAG) == 0)
	{
		if(BCryptDeriveKeyPBKDF2(hAlg, (PUCHAR)pass, (ULONG)passlen,
								  (PUCHAR)salt, (ULONG)saltlen,
								  (ULONGLONG)iter, out, (ULONG)outlen, 0) == 0)
			ok = 1;
		BCryptCloseAlgorithmProvider(hAlg, 0);
	}
	return ok;
}

// AES-256-CBC with PKCS7 padding (same default as EVP_Encrypt/DecryptInit_ex) -- returns output
// length on success, -1 on failure
static int aes_cbc_crypt(int encrypt, const unsigned char* key, const unsigned char* iv,
						  const unsigned char* in, int inlen, unsigned char* out, int outcap)
{
	BCRYPT_ALG_HANDLE hAlg = NULL;
	int result = -1;
	if(BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_AES_ALGORITHM, NULL, 0) != 0)
		return -1;
	if(BCryptSetProperty(hAlg, BCRYPT_CHAINING_MODE, (PUCHAR)BCRYPT_CHAIN_MODE_CBC,
						  sizeof(BCRYPT_CHAIN_MODE_CBC), 0) == 0)
	{
		BCRYPT_KEY_HANDLE hKey = NULL;
		if(BCryptGenerateSymmetricKey(hAlg, &hKey, NULL, 0, (PUCHAR)key, AES_KEY_LEN, 0) == 0)
		{
			unsigned char ivCopy[AES_IV_LEN];
			memcpy(ivCopy, iv, AES_IV_LEN);
			ULONG written = 0;
			NTSTATUS st;
			if(encrypt)
				st = BCryptEncrypt(hKey, (PUCHAR)in, (ULONG)inlen, NULL, ivCopy, AES_IV_LEN,
									out, (ULONG)outcap, &written, BCRYPT_BLOCK_PADDING);
			else
				st = BCryptDecrypt(hKey, (PUCHAR)in, (ULONG)inlen, NULL, ivCopy, AES_IV_LEN,
									out, (ULONG)outcap, &written, BCRYPT_BLOCK_PADDING);
			if(st == 0)
				result = (int)written;
			BCryptDestroyKey(hKey);
		}
	}
	BCryptCloseAlgorithmProvider(hAlg, 0);
	return result;
}
#else // orig code (OpenSSL)
static char* aes_b64_encode(const unsigned char* in, int inlen)
{
	int cap = 4 * ((inlen + 2) / 3) + 1;
	char* out = (char*)malloc(cap);
	if(!out) return NULL;
	int n = EVP_EncodeBlock((unsigned char*)out, in, inlen);
	out[n] = '\0';
	return out;
}

static unsigned char* aes_b64_decode(const char* in, int* outlen)
{
	int inlen = (int)strlen(in);
	char* clean = (char*)malloc(inlen + 1);
	if(!clean) return NULL;
	int c = 0;
	for(int i = 0; i < inlen; i++)
	{
		char ch = in[i];
		if(ch == '\n' || ch == '\r' || ch == ' ' || ch == '\t') continue;
		clean[c++] = ch;
	}
	clean[c] = '\0';
	if(c == 0 || (c % 4) != 0) { free(clean); return NULL; }
	int pad = 0;
	if(c >= 1 && clean[c-1] == '=') pad++;
	if(c >= 2 && clean[c-2] == '=') pad++;
	unsigned char* out = (unsigned char*)malloc((c / 4) * 3 + 1);
	if(!out) { free(clean); return NULL; }
	int n = EVP_DecodeBlock(out, (unsigned char*)clean, c);
	free(clean);
	if(n < 0) { free(out); return NULL; }
	*outlen = n - pad;
	return out;
}

static int aes_rand_bytes(unsigned char* buf, int len)
{
	return RAND_bytes(buf, len);
}

static int aes_derive_keyiv(KCSTR pass, int passlen, const unsigned char* salt, int saltlen,
							 int iter, unsigned char* out, int outlen)
{
	return PKCS5_PBKDF2_HMAC(pass, passlen, salt, saltlen, iter, EVP_sha256(), outlen, out) == 1 ? 1 : 0;
}

static int aes_cbc_crypt(int encrypt, const unsigned char* key, const unsigned char* iv,
						  const unsigned char* in, int inlen, unsigned char* out, int outcap)
{
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	if(!ctx) return -1;
	int len = 0, tmp = 0, ok = 1;
	if(encrypt)
	{
		if(EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) ok = 0;
		if(ok && EVP_EncryptUpdate(ctx, out, &len, in, inlen) != 1) ok = 0;
		if(ok && EVP_EncryptFinal_ex(ctx, out + len, &tmp) != 1) ok = 0;
	}
	else
	{
		if(EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1) ok = 0;
		if(ok && EVP_DecryptUpdate(ctx, out, &len, in, inlen) != 1) ok = 0;
		if(ok && EVP_DecryptFinal_ex(ctx, out + len, &tmp) != 1) ok = 0;
	}
	EVP_CIPHER_CTX_free(ctx);
	return ok ? (len + tmp) : -1;
}
#endif // <<< claude-code

KCSTR Aes256::encrypt(KSTRING _key, KSTRING & _plain, KSTRING & _result)
{
	_result = "";
	unsigned char salt[AES_SALT_LEN];
	if(!aes_rand_bytes(salt, AES_SALT_LEN)) return (KCSTR)_result;
	unsigned char keyiv[AES_KEY_LEN + AES_IV_LEN];
	if(!aes_derive_keyiv((KCSTR)_key, (int)_key.LENGTH(), salt, AES_SALT_LEN,
						  AES_PBKDF2_ITER, keyiv, AES_KEY_LEN + AES_IV_LEN))
		return (KCSTR)_result;

	int plainlen = (int)_plain.LENGTH();
	unsigned char* cipher = (unsigned char*)malloc(plainlen + AES_IV_LEN);
	if(!cipher) return (KCSTR)_result;

	int clen = aes_cbc_crypt(1, keyiv, keyiv + AES_KEY_LEN,
							  (const unsigned char*)(KCSTR)_plain, plainlen,
							  cipher, plainlen + AES_IV_LEN);
	if(clen < 0) { free(cipher); return (KCSTR)_result; }

	int rawlen = 8 + AES_SALT_LEN + clen;
	unsigned char* raw = (unsigned char*)malloc(rawlen);
	if(!raw) { free(cipher); return (KCSTR)_result; }
	memcpy(raw, AES_MAGIC, 8);
	memcpy(raw + 8, salt, AES_SALT_LEN);
	memcpy(raw + 8 + AES_SALT_LEN, cipher, clen);
	free(cipher);

	char* b64 = aes_b64_encode(raw, rawlen);
	free(raw);
	if(!b64) return (KCSTR)_result;
	_result = b64;
	free(b64);
	return (KCSTR)_result;
}

KCSTR Aes256::decrypt(KSTRING _key, KSTRING & _enc, KSTRING & _result)
{
	_result = "";
	int rawlen = 0;
	unsigned char* raw = aes_b64_decode((KCSTR)_enc, &rawlen);
	if(!raw) return (KCSTR)_result;

	if(rawlen < 8 + AES_SALT_LEN || memcmp(raw, AES_MAGIC, 8) != 0) { free(raw); return (KCSTR)_result; }

	unsigned char salt[AES_SALT_LEN];
	memcpy(salt, raw + 8, AES_SALT_LEN);
	unsigned char* cipher = raw + 8 + AES_SALT_LEN;
	int cipherlen = rawlen - 8 - AES_SALT_LEN;
	if(cipherlen <= 0 || (cipherlen % AES_IV_LEN) != 0) { free(raw); return (KCSTR)_result; }

	unsigned char keyiv[AES_KEY_LEN + AES_IV_LEN];
	if(!aes_derive_keyiv((KCSTR)_key, (int)_key.LENGTH(), salt, AES_SALT_LEN,
						  AES_PBKDF2_ITER, keyiv, AES_KEY_LEN + AES_IV_LEN))
	{ free(raw); return (KCSTR)_result; }

	unsigned char* plain = (unsigned char*)malloc(cipherlen + 1);
	if(!plain) { free(raw); return (KCSTR)_result; }

	int plen = aes_cbc_crypt(0, keyiv, keyiv + AES_KEY_LEN, cipher, cipherlen, plain, cipherlen + 1);
	free(raw);
	if(plen < 0) { free(plain); return (KCSTR)_result; }
	plain[plen] = '\0';
	_result = (KCSTR)plain;
	free(plain);
	return (KCSTR)_result;
}
void Aes256::test(KSTRING _input)
{
	const char * key = "!dorafather";
	const char * plain = "!dora-text";
	KSTRING pln = plain;
	KSTRING enc; KSTRING dec;
	printf("key = [%s], plain = [%s]\n",key,plain);
	printf("enc = [%s]\n",encrypt(key, pln, enc));
	printf("dec = [%s]\n",decrypt(key, enc, dec));
}
}
