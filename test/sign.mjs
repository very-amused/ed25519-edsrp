import fs from 'node:fs'
import crypto from 'node:crypto'

const file = fs.readFileSync('build/ed25519_edsrp.wasm')

;(async function() {
  const src = await WebAssembly.instantiate(file)
  /** @type {import('../src/ed25519').ED25519.Exports} */
  const ed25519 = src.instance.exports

	console.log('Generating random message')
  const messageLen = 32
  const messagePtr = ed25519.malloc(messageLen)
  const messageView = new Uint8Array(ed25519.memory.buffer, messagePtr, messageLen)
  crypto.randomFillSync(messageView)

	console.log('Generating ed25519 keypair')
  const seedLen = 32
  const seedPtr = ed25519.malloc(seedLen)
  const seedView = new Uint8Array(ed25519.memory.buffer, seedPtr, seedLen)
  crypto.randomFillSync(seedView)

  const privateKeyLen = 64
  const publicKeyLen = 32
  const privateKeyPtr = ed25519.malloc(privateKeyLen)
  const publicKeyPtr = ed25519.malloc(publicKeyLen)
  ed25519.ed25519_keypair(seedPtr, publicKeyPtr, privateKeyPtr)

	console.log('Signing message')
  const signedMessageLen = 64 + messageLen
  const signedMessagePtr = ed25519.malloc(signedMessageLen)
	const signedMessageView = new Uint8Array(ed25519.memory.buffer, signedMessagePtr, signedMessageLen)
  ed25519.ed25519_sign(signedMessagePtr, messagePtr, messageLen, privateKeyPtr)

  const encodedSignedMessage = Buffer.from(signedMessageView).toString('base64')
  const publicKeyView = new Uint8Array(ed25519.memory.buffer, publicKeyPtr, publicKeyLen)
  const encodedPublicKey = Buffer.from(publicKeyView).toString('base64')

  ed25519.free(seedPtr)
  ed25519.free(privateKeyPtr)
  ed25519.free(publicKeyPtr)
  ed25519.free(messagePtr)
  ed25519.free(signedMessagePtr)

  fs.writeFileSync('test/message.txt', `${encodedSignedMessage}\n${encodedPublicKey}`)
})()
