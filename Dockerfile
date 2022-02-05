FROM alpine:3.15.0 AS build
ENV DEBIAN_FRONTEND noninteractive
RUN apk --no-cache add cmake gcc g++ make libc-dev linux-headers
WORKDIR /home
COPY . .
RUN chmod +x ./external/ext-boost/bootstrap.sh && chmod +x ./external/ext-boost/libs/config/test/link/bc_gen.sh && chmod +x ./external/ext-boost/libs/config/test/link/common.sh && chmod +x ./external/ext-boost/libs/config/test/link/vc_gen.sh && chmod +x ./external/ext-boost/libs/predef/tools/ci/linux-cxx-install.sh && chmod +x ./external/ext-boost/libs/predef/tools/ci/macos-xcode-install.sh && chmod +x ./external/ext-boost/libs/program_options/ci/build.sh && chmod +x ./external/ext-boost/libs/program_options/ci/codecov.sh && chmod +x ./external/ext-boost/libs/program_options/ci/coverity.sh && chmod +x ./external/ext-boost/libs/program_options/ci/cppcheck.sh && chmod +x ./external/ext-boost/tools/build/bootstrap.sh && chmod +x ./external/ext-boost/tools/build/src/engine/build.sh && chmod +x ./external/openssl/demos/certs/apps/mkacerts.sh && chmod +x ./external/openssl/demos/certs/apps/mkxcerts.sh && chmod +x ./external/openssl/demos/certs/mkcerts.sh && chmod +x ./external/openssl/demos/certs/ocspquery.sh && chmod +x ./external/openssl/demos/certs/ocsprun.sh && chmod +x ./external/openssl/test/certs/mkcert.sh && chmod +x ./external/openssl/test/certs/setup.sh && chmod +x ./external/openssl/test/recipes/95-test_external_krb5_data/krb5.sh && chmod +x ./external/openssl/test/recipes/95-test_external_pyca_data/cryptography.sh && chmod +x ./external/openssl/test/smime-certs/mksmime-certs.sh && chmod +x ./external/openssl/util/opensslwrap.sh
RUN mkdir build && cd build && cmake .. && make -j4

FROM alpine:3.15.0
RUN apk --no-cache add libstdc++ libgcc
COPY --from=build /home/build/src/NumgleCpp/NumgleCpp /
CMD [ "/NumgleCpp" ]