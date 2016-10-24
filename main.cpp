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

USING_NAMESPACE(CryptoPP)
USING_NAMESPACE(std)
const int MAX_PHRASE_LENGTH=250;

void test_1()
{
    CryptoPP::MD5 hash;
    byte digest[ CryptoPP::MD5::DIGESTSIZE ];
    std::string message = "AGty";

    hash.CalculateDigest( digest, (const byte*)message.c_str(), message.length());

    CryptoPP::HexEncoder encoder;
    std::string output;
    encoder.Attach( new CryptoPP::StringSink( output ) );
    encoder.Put( digest, sizeof(digest) );
    encoder.MessageEnd();

    std::cout << "Input string: " << message << std::endl;
    std::cout << "MD5: " << output << std::endl;
}

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

void test_3()
{
    AutoSeededRandomPool prng;

    SecByteBlock key(AES::DEFAULT_KEYLENGTH);
    prng.GenerateBlock( key, key.size() );

    string plain = "ECB Mode Test";
    string cipher, encoded, recovered;

    /*********************************\
    \*********************************/

    try
    {
        cout << "plain text: " << plain << endl;

        ECB_Mode< AES >::Encryption e;
        e.SetKey( key, key.size() );

        // The StreamTransformationFilter adds padding
        //  as required. ECB and CBC Mode must be padded
        //  to the block size of the cipher.
        StringSource ss1( plain, true,
            new StreamTransformationFilter( e,
                new StringSink( cipher )
            ) // StreamTransformationFilter
        ); // StringSource
    }
    catch( CryptoPP::Exception& e )
    {
        cerr << e.what() << endl;
        exit(1);
    }

    /*********************************\
    \*********************************/

    // Pretty print cipher text
    StringSource ss2( cipher, true,
        new HexEncoder(
            new StringSink( encoded )
        ) // HexEncoder
    ); // StringSource
    cout << "cipher text: " << encoded << endl;

    /*********************************\
    \*********************************/

    try
    {
        ECB_Mode< AES >::Decryption d;
        // ECB Mode does not use an IV
        d.SetKey( key, key.size() );

        // The StreamTransformationFilter removes
        //  padding as required.
        StringSource ss3( cipher, true,
            new StreamTransformationFilter( d,
                new StringSink( recovered )
            ) // StreamTransformationFilter
        ); // StringSource

        cout << "recovered text: " << recovered << endl;
    }
    catch( CryptoPP::Exception& e )
    {
        cerr << e.what() << endl;
        exit(1);
    }
}

std::string encrypt(const std::string& str_in, const std::string& key)
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

std::string decrypt(const std::string& str_in, const std::string& key)
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
            str_encrypted = encrypt(str, key);
            str_decrypted = decrypt(str_encrypted, key);
        }
        catch (const CryptoPP::Exception& e) {
           std::cerr << e.what() << std::endl;
        }

        std::cout << "str_encrypted: " << str_encrypted << std::endl;
        std::cout << "str_decrypted: " << str_decrypted << std::endl;
}


// new version pf KP
void test_6()
{
    AutoSeededRandomPool prng;

	//see file
    Integer n("");
    Integer e("");
    Integer d("");

    /////////////////////////////////////////////////////////
    RSA::PublicKey pubKey;
    pubKey.Initialize(n, e);

    RSA::PrivateKey privKey;
    privKey.Initialize(n, e, d);
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
