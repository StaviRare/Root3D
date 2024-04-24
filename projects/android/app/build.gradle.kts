plugins {
    id("com.android.application")
}

android {
    namespace = "com.example.root3d"
    compileSdk = 34
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
    }

    // Java sources
    sourceSets {
        getByName("main") {
            java.srcDir("src/main/java/")
            java.srcDir("../../../source/android/java")
        }
    }

    // Native sources
    externalNativeBuild {
        cmake {
            path = file("../../../source/android/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
}


// Copy resources asset
val copyResources by tasks.registering(Copy::class) {
    description = "Copies resources to the assets directory"
    from("../../../assets/packed")
    into("src/main/assets")
    include("resources.bin")
}

tasks.named("preBuild") {
    dependsOn(copyResources)
}

// ToDo - Find a better way
gradle.buildFinished {
    delete("src/main/assets/resources.bin")
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
