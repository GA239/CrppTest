#include <iostream>

#define CRYPTOPP_DEFAULT_NO_DLL
#include <cryptopp/dll.h>
#ifdef CRYPTOPP_WIN32_AVAILABLE
#include <windows.h>
#endif
#include <cryptopp/md5.h>
#include <cryptopp/rsa.h>
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/base64.h>

#include "support.h"
#include <QSettings>
#include <QVariant>

#include <QDebug>

USING_NAMESPACE(CryptoPP)
USING_NAMESPACE(std)
const int MAX_PHRASE_LENGTH=250;


void test_2()
{
    ///////////////////////////////////////
    // Pseudo Random Number Generator
    CryptoPP::AutoSeededRandomPool rng;

    ///////////////////////////////////////
    // Generate Parameters
    CryptoPP::InvertibleRSAFunction params;
    //CryptoPP::RSAFunction p;
    //p.Initialize();
    params.GenerateRandomWithKeySize(rng, 3072);


    ///////////////////////////////////////
    // Generated Parameters
    const Integer& _n = params.GetModulus();
    const Integer& _p = params.GetPrime1();
    const Integer& _q = params.GetPrime2();
    const Integer& _d = params.GetPrivateExponent();
    const Integer& _e = params.GetPublicExponent();

    ///////////////////////////////////////
    // Dump
    cout << "RSA Parameters:" << endl;
    cout << " n: " << _n << endl;
    cout << " p: " << _p << endl;
    cout << " q: " << _q << endl;
    cout << " d: " << _d << endl;
    cout << " e: " << _e << endl;
    cout << endl;


    ///////////////////////////////////////
    // Create Keys
    CryptoPP::RSA::PrivateKey privateKey(params);
    CryptoPP::RSA::PublicKey publicKey(params);

    string plain="RSA AG", cipher, recovered;
    cout << "plain text   " <<plain<< endl;

    ////////////////////////////////////////////////
    // Encryption
    CryptoPP::RSAES_OAEP_SHA_Encryptor e(publicKey);

    CryptoPP::StringSource ss1(plain, true,
                     new PK_EncryptorFilter(rng, e,
                     new StringSink(cipher)) // PK_EncryptorFilter
                     ); // StringSource

    //cout << "cipher plain text   " <<cipher<< endl;

    ////////////////////////////////////////////////
    // Decryption
    CryptoPP::RSAES_OAEP_SHA_Decryptor d(privateKey);

    CryptoPP::StringSource ss2(cipher, true,
                               new PK_DecryptorFilter(rng, d,
                               new StringSink(recovered)
                               ) // PK_DecryptorFilter
                               ); // StringSource
    cout << "Recovered plain text   " <<recovered<< endl;
}


std::string AESencrypt(const std::string& str_in, const std::string& key)
{
    std::string str_out;
    CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption encryption((byte*)key.c_str(), key.length());

    CryptoPP::StringSource encryptor(str_in, true,
            new CryptoPP::StreamTransformationFilter(encryption,
                new CryptoPP::Base64Encoder(
                    new CryptoPP::StringSink(str_out),
                    false // do not append a newline
                )
            )
    );
    return str_out;
}

std::string AESdecrypt(const std::string& str_in, const std::string& key)
{
    std::string str_out;

    CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption decryption((byte*)key.c_str(), key.length());

    CryptoPP::StringSource decryptor(str_in, true,
        new CryptoPP::Base64Decoder(
                new CryptoPP::StreamTransformationFilter(decryption,
                    new CryptoPP::StringSink(str_out)
                )
        )
    );
    return str_out;
}

void test_4()
{
    std::string str = "hello";
    std::string key = "13407807929942597099574024998205846127479365820592393377716219260641510296154601176461787345503903833566711934434611064367503005804480173780334406043355239";

        std::string str_encrypted, str_decrypted;

        try {
            str_encrypted = AESencrypt(str, key);
            str_decrypted = AESdecrypt(str_encrypted, key);
        }
        catch (const CryptoPP::Exception& e) {
           std::cerr << e.what() << std::endl;
        }

        std::cout << "str_encrypted: " << str_encrypted << std::endl;
        std::cout << "str_decrypted: " << str_decrypted << std::endl;
}

RSA::PublicKey createPublicKey(QString pkey)
{
    Support* sp = new Support;
    sp->settings = new QSettings("../CrypToPPTest/public.key",QSettings::IniFormat);
    Integer n(sp->settings->value("public/n",0).toString().toUtf8().constData());
    Integer e(sp->settings->value("public/e",0).toString().toUtf8().constData());
    delete sp;
    RSA::PublicKey pubKey;
    pubKey.Initialize(n, e);
    return pubKey;
}

RSA::PrivateKey createPrivateKey(QString prkey)
{
    Support* sp = new Support;
    sp->settings = new QSettings("../CrypToPPTest/private.key",QSettings::IniFormat);
    Integer n(sp->settings->value("public/n",0).toString().toUtf8().constData());
    Integer e(sp->settings->value("public/e",0).toString().toUtf8().constData());
    Integer d(sp->settings->value("private/d",0).toString().toUtf8().constData());
    RSA::PrivateKey privKey;
    privKey.Initialize(n, e, d);
    /////////////////////////////////////////////////////////
    delete sp;
    return privKey;
}

// new version pf KP
void test_6()
{
    AutoSeededRandomPool prng;

    /////////////////////////////////////////////////////////
    RSA::PublicKey pubKey = createPublicKey("sd");
    ///pubKey.Initialize(n, e);

    RSA::PrivateKey privKey = createPrivateKey("sd");
    ///privKey.Initialize(n, e, d);
    /////////////////////////////////////////////////////////

    string message, recovered;
    Integer m, c, r;

    message = "hello my name is andrew. Itis very secret message thanksssssstds";
    cout << "message: " << message << endl;

    // Treat the message as a big endian array
    m = Integer((const byte *)message.data(), message.size());
    cout << "m: " << hex << m << endl;

    // Encrypt
    c = pubKey.ApplyFunction(m);
    cout << "c: " << hex << c << endl;

    // Decrypt
    r = privKey.CalculateInverse(prng, c);
    cout << "r: " << hex << r << endl;

    // Round trip the message
    size_t req = r.MinEncodedSize();
    recovered.resize(req);
    r.Encode((byte *)recovered.data(), recovered.size());

    cout << "recovered: " << recovered << endl;
    return;
}

int main(int argc, char *argv[])
{
    test_6();
    return 0;
}
