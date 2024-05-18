plugins {
    id("com.android.application")
}

android {
    compileSdk = 34
    ndkVersion = "25.1.8937393"
    namespace = "com.example.root3d"
    buildDir = file("../../../build/android")

    defaultConfig {
        applicationId = "com.example.root3d"
        minSdk = 16
        targetSdk = 34
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }

    buildFeatures {
        viewBinding = true
        flavorDimensions += "architecture"
    }

    productFlavors {
        create("arm7") {
            dimension = "architecture"
            ndk {
                abiFilters.add("armeabi-v7a")
            }
        }
        create("arm8") {
            dimension = "architecture"
            ndk {
                abiFilters.add("arm64-v8a")
            }
        }
        create("x86") {
            dimension = "architecture"
            ndk {
                abiFilters.add("x86")
            }
        }
        create("universal") {
            dimension = "architecture"
            ndk {
                abiFilters.add("x86")
                abiFilters.add("arm64-v8a")
                abiFilters.add("armeabi-v7a")
            }
        }

        buildOutputs.all {
            val variantOutputImpl = this as com.android.build.gradle.internal.api.BaseVariantOutputImpl
            variantOutputImpl.outputFileName =  "root3d.apk" // For build scripts. We're leaving it like this for now.
        }
    }

    // Java sources
    sourceSets {
        getByName("main") {
            java.srcDir("../../../source/app/android/java/")
        }
    }

    // Native sources
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
}

tasks {
    val copyResources by registering(Copy::class) {
        description = "Copies resources to the assets directory"
        from("../../../assets/packed")
        into("src/main/assets")
        include("resources.bin")
    }

    named("preBuild") {
        dependsOn(copyResources)
    }
}

dependencies {
    // AndroidX and Material Components
    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("com.google.android.material:material:1.10.0")
    implementation("androidx.constraintlayout:constraintlayout:2.1.4")

    // Testing dependencies
    testImplementation("junit:junit:4.13.2")
    androidTestImplementation("androidx.test.ext:junit:1.1.5")
    androidTestImplementation("androidx.test.espresso:espresso-core:3.5.1")
}
