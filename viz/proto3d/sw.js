// Cache-first service worker for offline use in the car -- once loaded
// once with a signal, everything needed (three.js, controls, config, the
// app itself) is cached, so it keeps working with zero connectivity.
// Bump CACHE_NAME whenever the app shell changes so clients pick up fresh
// files instead of serving stale cached ones forever.
const CACHE_NAME = "takumicup-v1";
const PRECACHE_URLS = [
  "./",
  "./index.html",
  "./config.js",
  "./manifest.json",
  "./icon-192.png",
  "./icon-512.png",
  "./vendor/three/build/three.module.js",
  "./vendor/three/examples/jsm/controls/OrbitControls.js",
];

self.addEventListener("install", (event) => {
  event.waitUntil(
    caches.open(CACHE_NAME).then((cache) => cache.addAll(PRECACHE_URLS)).then(() => self.skipWaiting())
  );
});

self.addEventListener("activate", (event) => {
  event.waitUntil(
    caches.keys().then((keys) =>
      Promise.all(keys.filter((k) => k !== CACHE_NAME).map((k) => caches.delete(k)))
    ).then(() => self.clients.claim())
  );
});

self.addEventListener("fetch", (event) => {
  if (event.request.method !== "GET") return;
  event.respondWith(
    caches.match(event.request).then((cached) => {
      if (cached) return cached;
      return fetch(event.request).then((response) => {
        if (response.ok) {
          const clone = response.clone();
          caches.open(CACHE_NAME).then((cache) => cache.put(event.request, clone));
        }
        return response;
      }).catch(() => cached); // offline and not cached: nothing we can do
    })
  );
});
