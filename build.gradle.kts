plugins {
    `java-library`
    kotlin("jvm")
}

group = "site.neworld"
val moduleName by extra("site.neworld.cio")

repositories {
    mavenCentral()
}

dependencies {
    api(project(":modules:utils"))
    implementation(kotlin("stdlib"))
    implementation("org.jetbrains.kotlinx:kotlinx-coroutines-core:1.4.3")
}

java {
    sourceCompatibility = JavaVersion.VERSION_16
    targetCompatibility = JavaVersion.VERSION_16
    modularity.inferModulePath.set(true)
}

tasks {
    compileJava {
        inputs.property("moduleName", moduleName)
        options.compilerArgs = listOf(
            "--add-modules", "jdk.incubator.foreign",
            "--patch-module", "$moduleName=${sourceSets.main.get().output.asPath}"
        )
    }
    compileKotlin {
        kotlinOptions {
            jvmTarget = "16"
            freeCompilerArgs = listOf("-Xopt-in=kotlin.RequiresOptIn")
        }
    }
}