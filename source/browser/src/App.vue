<template>
  <v-app>
    <v-app-bar
      app
      color="#17a2b7"
      dark
    >
      <div class="d-flex align-center">
        <v-img
          alt="LION Logo"
          class="shrink mr-2"
          contain
          src="./assets/LION Logo.png"
          transition="scale-transition"
          width="135"
        />

        <span> Resource Browser </span>
      </div>

      <v-spacer></v-spacer>
    </v-app-bar>

    <v-main>
      <Home/>
    </v-main>
  </v-app>
</template>

<script>
import Home from './components/Home';

export default {
  name: 'App',

  components: {
    Home,
  },

  data: () => ({
    //
  }),
};
</script>

<style>
span {
  opacity: 0.8;
}
</style>