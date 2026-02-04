/** @type {import('tailwindcss').Config} */
export default {
  content: [
    "./index.html",
    "./src/**/*.{js,jsx,ts,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        brand: {
          50: "#f0f7ff",
          100: "#dbeafe",
          500: "#2563eb",
          600: "#1d4ed8",
          900: "#0b1220",
        },
      },
    },
  },
  plugins: [],
};